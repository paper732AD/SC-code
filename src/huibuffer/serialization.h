#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <vector>
#include <sys/uio.h>
#include <iostream>
#include "iovector.h"
#include "utility.h"
#include "util/logger.h"
#include "pkthdr.h"
#define PRINTF(s) \
    std::cout<< s << std::endl;
#define PRINT(s) \
    std::cout<< s;

namespace erpc
{

    struct buffer
    {
        void* _ptr = nullptr;
        size_t _len = 0;
        buffer() { }
        buffer(void* buf, size_t size) : _ptr(buf), _len(size) { }
        void* addr() const { return _ptr; }
        size_t size() const { return _len; }
        size_t length() const { return size(); }
        void assign(const void* ptr, size_t len) { _ptr = (void*)ptr; _len = len; }
    };

    // aligned buffer, which will be processed in the 1st pass of
    // serialization / deserialization process, helping to ensure alignment
    struct aligned_buffer : public buffer
    {
    };

    template<typename T>
    struct fixed_buffer : public buffer
    {
        using buffer::buffer;
        fixed_buffer(const T* x) : buffer(x, sizeof(*x)) { }
        void assign(const T* x) { buffer::assign(x, sizeof(*x)); }
        const T* get() const { return (T*)addr(); }
    };

    template<typename T>
    struct array : public buffer
    {
        array() { }
        array(const T* ptr, size_t size) { assign(ptr, size); }
        array(const std::vector<T>& vec) { assign(vec); }
        size_t size() const { return _len / sizeof(T); }
        const T* begin() const    { return (T*)_ptr; }
        const T* end() const      { return begin() + size(); }
        const T& operator[](long i) const { return ((char*)_ptr)[i]; }
        const T& front() const    { return (*this)[0]; }
        const T& back() const     { return (*this)[(long)size() - 1]; }
        void assign(const T* x, size_t size) { buffer::assign(x, sizeof(*x) * size); }
        void assign(const std::vector<T>& vec)
        {
            assign(&vec[0], vec.size());
        }

        struct iterator {
            T* p;
            iterator(T* t):p(t) {}
            const T& operator*() {
                return *p;
            }
            iterator& operator++(int) {
                p++;
                return *this;
            }
            bool operator!=(iterator rhs) {
                return p != rhs.p;
            }
            bool operator==(iterator rhs) {
                return p == rhs.p;
            }
        };
    };

    struct string : public array<char>
    {
        using base = array<char>;
        using base::base;

        template<size_t LEN>
        string(const char (&s)[LEN]) : base(s, LEN) { }
        string(const char* s) : base(s, strlen(s)+1) { }
        string(const std::string& s) { assign(s); }
        string() : base(nullptr, 0) { }
        const char* c_str() const { return begin(); }
        operator const char* () const { return c_str(); }
        void assign(const char* s) {
            base::assign(s, strlen(s) );
        }
        void assign(const std::string& s)
        {
            array<char>::assign(s.c_str(), s.size());
        }
    };

    struct iovec_array : public array<iovec>
    {
        // using base = array<iovec>;
        // using base::base;
        size_t summed_size;
        iovec_array() { }
        iovec_array(iovec* iov, int iovcnt)
        {
            assign(iov, iovcnt);
        }
        size_t assign(iovec* iov, int iovcnt)
        {
            assert(iovcnt >= 0);
            array<iovec>::assign(iov, (size_t)iovcnt);
            return sum();
        }
        size_t sum()
        {
            summed_size = 0;
            for (auto& v: *this)
                summed_size += v.iov_len;
            return summed_size;
        }
    };

    // Represents an iovec[] that has aligned iov_bases and iov_lens.
    // It will be processed in the 1st pass of serialization /
    // deserialization process, helping to ensure alignment.
    struct aligned_iovec_array : public iovec_array
    {
    };

    // structs of concrete messages MUST derive from `Message`,
    // and implement serialize_fields(), possbile with SERIALIZE_FIELDS()
    struct Message
    {
    public:
        template<typename AR>
        void serialize_fields(AR& ar)
        {
        }

    protected:
        template<typename AR>
        void reduce(AR& ar)
        {
        }
        template<typename AR, typename T, typename...Ts>
        void reduce(AR& ar, T& x, Ts&...xs)
        {
            ar.process_field(x);
            reduce(ar, xs...);// 依次处理
        }
    };

#define PROCESS_FIELDS(...)                     \
        template<typename AR>                   \
        void process_fields(AR& ar) {           \
            return reduce(ar, __VA_ARGS__);     \
        }



    template<typename Derived>  // The Curiously Recurring Template Pattern (CRTP)
    class ArchiveBase
    {
    public:
        Derived* d()
        {
            return static_cast<Derived*>(this);
        }
        void noop(){

        }

        // template<typename T>
        // void process_field(T& x)
        // {
        // }

        void process_field(buffer& x)
        {
            assert("must be re-implemented in derived classes");
        }

        template<typename T>
        void process_field(fixed_buffer<T>& x)
        {
            static_assert(
                !std::is_base_of<Message, T>::value,
                "no Messages are allowed");

            d()->process_field((buffer&)x);
            d()->process_field(*x.get());
        }

        template<typename T>
        void process_field(array<T>& x)
        {
            static_assert(
                !std::is_base_of<Message, T>::value,
                "no Messages are allowed");
            ERPC_INFO("In process_field(array<T>& x);");
            d()->process_field((buffer&)x);
            for (auto& i: x)
                d()->process_field(i);
        }

        void process_field(string& x)
        {
            ERPC_INFO("process_field(string& x)\n");
            d()->process_field((buffer&)x);
            ERPC_INFO("process_field(string& x) finish\n");
        }

        void process_field(iovec_array& x)
        {
            assert("must be re-implemented in derived classes");
        }

        // overload for embedded Message
        template<typename T>
        void process_field(T& x)
        {
            //overload for embedded Message
            //std::cout << std::is_base_of<Message, T>::value << std::endl;
            if(std::is_base_of<Message, T>::value){
                x.serialize_fields(*d());
            }
        }
    };

    template<typename T>
    struct _FilterAlignedFields
    {
        T* _obj;
        bool _flag;
        void process_field(aligned_buffer& x)
        {
            if (_flag)
                _obj->proceass_field((buffer&)x);
        }
        void process_field(aligned_iovec_array& x)
        {
            if (_flag)
                _obj->process_field((iovec_array&)x);
        }
        template<typename P>
        void process_field(P& x)
        {
            // * 处理对齐字段时候，不进这个；处理非对齐字段时候，进入这个。
            if (!_flag)
                _obj->process_field(x);
        }
    };

    template<typename T>
    _FilterAlignedFields<T> FilterAlignedFields(T* obj, bool flag)
    {
        return _FilterAlignedFields<T>{obj, flag};
    }

    class Serializer : public ArchiveBase<Serializer>
    {
    public:
        // Serializer(){
        //     ERPC_WARN("Serializer()\n");
        // }
        IOVector iov;
        uint32_t total_pkg;
        IOVector t_iov;
        uint32_t data_pkg_index[2000];//两个包会有三条记录
        //  0，2，3; 0-1是第一个包，2-2是第二个包。
        //  0，2，4; 0-1是第一个包，2-3是第二个包。
        bool iovfull = false;
        bool is_sick = false;
        uint32_t used_size;
        uint8_t* new_buff;
        using ArchiveBase<Serializer>::process_field;

        pkthdr_t* get_n_hdr (uint32_t n) const {//n= 0,1,2,3,4...
            assert(n <= static_cast<pkthdr_t*>(iov[0].iov_base)->total_pkt);
            return static_cast<pkthdr_t*>(iov[n].iov_base);
        }

        iovec* get_n_data(size_t n, uint32_t& iovec_len) { // n= 0,1,2,3,4....
            assert(n<total_pkg);
            iovec_len = data_pkg_index[n+1] - data_pkg_index[n];
            return &iov[total_pkg + data_pkg_index[n]];//total_pkg: iov of pkthdr; data_pkg_index[n] is the index for data
        }

               // return total package //**Important**//
        uint32_t init_data_iov(int maxDataPerPkt){
            if(this->iov.sum() <= maxDataPerPkt){ // 一个包的情况
                total_pkg = 1;
                data_pkg_index[0] = 0;
                data_pkg_index[1] = this->iov.iovcnt();
                return total_pkg;
            }

            t_iov.clear();
            for(int i=0;i<iov.iovcnt();++i){
                t_iov.push_back(iov[i]);
            }
            
            IOVector& _iov = this->iov;
            size_t sum =  t_iov.sum();
            int add = 0;
            if(sum%maxDataPerPkt) add=1; // 不够整除,后面加一个额外的包
            _iov.clear();
            _iov.iov_begin = _iov.iov_end = t_iov.sum() / maxDataPerPkt + add;


            uint32_t& data_index = total_pkg;
            data_index = 0;
            int temp = maxDataPerPkt;
            for(int i = 0;i<t_iov.iovcnt();++i){
                if(t_iov[i].iov_len <= 0) // 比如空的string
                    continue;
                iovec& _t_iov = t_iov[i];
                while(_t_iov.iov_len > 0){
                    if(temp == maxDataPerPkt){ // a new package;
                        data_pkg_index[data_index++] = _iov.iovcnt();// record a new package;
                        ERPC_INFO("0data_pkg_index[data_index++]=_iov.iovcnt()(%d)\n", _iov.iovcnt());
                    }
                    if(temp>_t_iov.iov_len){
                        ERPC_INFO("1temp = %d, iov[i].iov_base = 0x%X, iov[i].iov_len = %d\n",temp ,_t_iov.iov_base ,_t_iov.iov_len);
                        _iov.push_back(_t_iov.iov_base, _t_iov.iov_len);
                        temp-=_t_iov.iov_len;
                        _t_iov.iov_base = static_cast<uint8_t*>(_t_iov.iov_base)+_t_iov.iov_len;
                        _t_iov.iov_len = 0;
                    }
                    else if(temp<_t_iov.iov_len){
                        ERPC_INFO("2temp = %d, t_iov[i].iov_base = 0x%X, t_iov[i].iov_len = %d\n",temp ,_t_iov.iov_base ,_t_iov.iov_len);
                        _iov.push_back(_t_iov.iov_base, temp);
                        _t_iov.iov_base = static_cast<uint8_t*>(_t_iov.iov_base)+temp;
                        _t_iov.iov_len -= temp;
                        temp = maxDataPerPkt;
                    }
                    else if(temp == _t_iov.iov_len){
                        ERPC_INFO("3temp = %d, t_iov[i].iov_base = 0x%X, t_iov[i].iov_len = %d\n",temp ,_t_iov.iov_base ,_t_iov.iov_len);
                        _iov.push_back(_t_iov.iov_base, _t_iov.iov_len);
                        temp = maxDataPerPkt;
                        _t_iov.iov_base = static_cast<uint8_t*>(_t_iov.iov_base)+_t_iov.iov_len;
                        _t_iov.iov_len = 0;
                    }
                    
                }
            }
            data_pkg_index[data_index] = _iov.iovcnt(); // 加一个虚拟的包作为最后，实际上没有这个长度。避免0，2，2这种情况；变成0，2，3。
            ERPC_INFO("there is data_index(%d) packages and %d iovs\n", data_index, _iov.iovcnt());
            // this->iov.iov_begin = this->iov.iov_end = data_index;
            // for(int i=0;i<_iov.iovcnt();++i){
            //     this->iov.push_back(_iov[i]);
            // }
            return data_index; // total pkg.
        }

        void process_field(buffer& x)
        {
            ERPC_INFO("int void process_field(buffer& x); iov.capacity = %d, iov_end=%d\n", iov.capacity, iov.iov_end);
            int tmp = iov.back_free_iovcnt();
            ERPC_INFO("return of iov.back_free_iovcnt() is %d\n",tmp);
            if ( tmp > 0) {
                if (x.size()>0)
                    iov.push_back(x.addr(), x.size());
            } else {
                iovfull = true;
                PRINTF("iovfull = true;")
            }
        }

        void process_field(iovec_array& x)
        {
            x.summed_size = 0;
            for (auto& v: x)
            {
                x.summed_size += v.iov_len;
                buffer buf(v.iov_base, v.iov_len);
                d()->process_field(buf);
            }
        }

        template<typename T>
        void serialize(T& x)
        {
            static_assert(
                std::is_base_of<Message, T>::value,
                "only Messages are permitted");
            ERPC_INFO("iov.capacity = %d\n", iov.capacity); // 是否是Message的子类

            // serialize aligned fields, non-aligned fields, and the main body
            auto aligned = FilterAlignedFields(this, true);
            x.process_fields(aligned); // 宏定义
            auto non_aligned = FilterAlignedFields(this, false);
            x.process_fields(non_aligned);
            buffer msg(&x, sizeof(x));
            d()->process_field(msg);
        }

        void clear (){
            iov.iov_begin = 1;
            iov.iov_end = 1;
            this->used_size = 0;
            this->is_sick = false;
            if(this->new_buff!=nullptr){
                delete(this->new_buff);
                this->new_buff = nullptr;
                // this->new_buff
            }
        }
    };

    class Deserializer : public ArchiveBase<Deserializer>
    {
    public:
        iovector* _iov;
        bool failed = false;

        using ArchiveBase<Deserializer>::process_field;

        void process_field(buffer& x)
        {
            ERPC_INFO("process_field(buffer& x), x.size() = %d\n", x.size());
            if (x.size()==0)
                return;
            x._ptr = _iov->extract_front_continuous(x.size());
            ERPC_INFO("iov.front().iov_len = %d\n",_iov->front().iov_len);
            if (!x._ptr)
                failed = true;
        }

        void process_field(iovec_array& x)
        {
            iovector_view v;
            ssize_t ret = _iov->extract_front(x.summed_size, &v);
            if (ret == (ssize_t)x.summed_size) {
                ERPC_INFO("process_field(iovec_array& x), x.summed_size = %d\n", x.summed_size);
                x.assign(v.iov, v.iovcnt);
            } else {
                failed = true;
            }
        }

        template<typename T>
        T* deserialize(iovector* iov)
        {
            static_assert(
                std::is_base_of<Message, T>::value,
                "only Messages are permitted");

            // deserialize the main body from back
            _iov=iov;
            auto t = iov -> extract_back<T>();
            if (t) {
                // deserialize aligned fields, and non-aligned fields, from front
                auto aligned = FilterAlignedFields(this, true);
                t->process_fields(aligned);
                ERPC_INFO("non_aligned\n");
                auto non_aligned = FilterAlignedFields(this, false);
                t->process_fields(non_aligned);
            } else {
                failed = true;
            }
            return t;
        }
    };
}

