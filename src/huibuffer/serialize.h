#pragma once
#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <vector>
#include <sys/uio.h>
#include "iovector.h"
#include <iostream>
#include "utility.h"

namespace RPC
{

#define PRINTF(s) \
    std::cout << s << std::endl;
#define PRINT(s) \
    std::cout << s;

    struct buffer
    {
        void *_ptr = nullptr;
        size_t _len = 0;
        buffer() {}
        buffer(void *buf, size_t size) : _ptr(buf), _len(size) {}
        void *addr() const { return _ptr; }
        size_t size() const { return _len; }
        size_t length() const { return size(); }
        void assign(const void *ptr, size_t len)
        {
            _ptr = (void *)ptr;
            _len = len;
        }
    };

    // aligned buffer, which will be processed in the 1st pass of
    // serialization / deserialization process, helping to ensure alignment
    struct aligned_buffer : public buffer
    {
    };

    template <typename T>
    struct fixed_buffer : public buffer
    {
        using buffer::buffer;
        fixed_buffer(const T *x) : buffer(x, sizeof(*x)) {}
        void assign(const T *x) { buffer::assign(x, sizeof(*x)); }
        const T *get() const { return (T *)addr(); }
    };

    template <typename T>
    struct array : public buffer
    {
        array() {}
        array(const T *ptr, size_t size) { assign(ptr, size); }
        array(const std::vector<T> &vec) { assign(vec); }
        size_t size() const { return _len / sizeof(T); }
        const T *begin() const { return (T *)_ptr; }
        const T *end() const { return begin() + size(); }
        const T &operator[](long i) const { return ((char *)_ptr)[i]; }
        const T &front() const { return (*this)[0]; }
        const T &back() const { return (*this)[(long)size() - 1]; }
        void assign(const T *x, size_t size) { buffer::assign(x, sizeof(*x) * size); }
        void assign(const std::vector<T> &vec)
        {
            assign(&vec[0], vec.size());
        }

        struct iterator
        {
            T *p;
            iterator(T *t) : p(t) {}
            const T &operator*()
            {
                return *p;
            }
            iterator &operator++(int)
            {
                p++;
                return *this;
            }
            bool operator!=(iterator rhs)
            {
                return p != rhs.p;
            }
            bool operator==(iterator rhs)
            {
                return p == rhs.p;
            }
        };
    };

    struct string : public array<char>
    {
        using base = array<char>;
        using base::base;

        template <size_t LEN>
        string(const char (&s)[LEN]) : base(s, LEN) {}
        string(const char *s) : base(s, strlen(s) + 1) {}
        string(const std::string &s) { assign(s); }
        string() : base(nullptr, 0) {}
        const char *c_str() const { return begin(); }
        operator const char *() const { return c_str(); }
        void assign(const char *s)
        {
            base::assign(s, strlen(s) + 1);
        }
        void assign(const std::string &s)
        {
            array<char>::assign(s.c_str(), s.size());
        }
    };

    struct iovec_array : public array<iovec>
    {
        // using base = array<iovec>;
        // using base::base;
        size_t summed_size;
        iovec_array() {}
        iovec_array(iovec *iov, int iovcnt)
        {
            assign(iov, iovcnt);
        }
        size_t assign(iovec *iov, int iovcnt)
        {
            assert(iovcnt >= 0);
            array<iovec>::assign(iov, (size_t)iovcnt);
            return sum();
        }
        size_t sum()
        {
            summed_size = 0;
            for (auto &v : *this)
            {
                summed_size += v.iov_len;
            }
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
        template <typename AR>
        void serialize_fields(AR &ar)
        {
            // PRINTF("没用~");
        }

    protected:
        template <typename AR>
        void reduce(AR &ar)
        {
        }
        template <typename AR, typename T, typename... Ts>
        void reduce(AR &ar, T &x, Ts &...xs)
        {
            ar.process_field(x);
            reduce(ar, xs...);
        }
    };

#define PROCESS_FIELDS(...)             \
    template <typename AR>              \
    void process_fields(AR &ar)         \
    {                                   \
        return reduce(ar, __VA_ARGS__); \
    }

    template <typename Derived> // The Curiously Recurring Template Pattern (CRTP)
    class ArchiveBase
    {
    public:
        Derived *d()
        {
            return static_cast<Derived *>(this);
        }

        void noop()
        {
        }

        // template<typename T>
        // void process_field(T& x)
        // {
        // }

        void process_field(buffer &x)
        {
            assert("must be re-implemented in derived classes");
        }

        template <typename T>
        void process_field(fixed_buffer<T> &x)
        {
            static_assert(
                !std::is_base_of<Message, T>::value,
                "no Messages are allowed");

            d()->process_field((buffer &)x);
            d()->process_field(*x.get());
        }

        template <typename T>
        void process_field(array<T> &x)
        {
            // PRINTF("process_field<array>");
            static_assert(
                !std::is_base_of<Message, T>::value,
                "no Messages are allowed");

            d()->process_field((buffer &)x);
            for (auto &i : x)
                d()->process_field(i);
        }

        void process_field(string &x)
        {
            // PRINT("process_field(string)");
            // PRINT(", address = ");
            // PRINTF(uint64_t(x.c_str()));
            d()->process_field((buffer &)x);
        }

        void process_field(iovec_array &x)
        {
            // PRINTF("process_field(iovec_array)");
            assert("must be re-implemented in derived classes");
        }

        // overload for embedded Message
        template <typename T>
        void process_field(T &x)
        {
            // overload for embedded Message
            // std::cout << std::is_base_of<Message, T>::value << std::endl;
            if (std::is_base_of<Message, T>::value)
            {
                x.serialize_fields(*d());
            }
        }
    };

    template <typename T>
    struct _FilterAlignedFields
    {
        T *_obj;
        bool _flag;
        void process_field(aligned_buffer &x)
        {
            if (_flag)
                _obj->process_field((buffer &)x);
        }
        void process_field(aligned_iovec_array &x)
        {
            if (_flag)
                _obj->process_field((iovec_array &)x);
        }
        template <typename P>
        void process_field(P &x)
        {
            // * 处理对齐字段时候，不进这个；处理非对齐字段时候，进入这个。
            if (!_flag)
                _obj->process_field(x);
        }
    };

    template <typename T>
    _FilterAlignedFields<T> FilterAlignedFields(T *obj, bool flag)
    {
        // RPC::SerializerIOV *
        return _FilterAlignedFields<T>{obj, flag};
    }

    class SerializerIOV : public ArchiveBase<SerializerIOV>
    {
    public:
        IOVector iov;
        bool iovfull = false;
        uint32_t total_pkg;
        uint32_t data_pkg_index[2000]; // 两个包会有三条记录

        using ArchiveBase<SerializerIOV>::process_field;

        void process_field(buffer &x)
        {
            if (iov.back_free_iovcnt() > 0)
            {
                // PRINT("x.size() = ");
                // PRINTF(x.size());
                if (x.size() > 0)
                    iov.push_back(x.addr(), x.size());
            }
            else
            {
                // PRINTF("iovfull = true");
                iovfull = true;
            }
        }

        void process_field(iovec_array &x)
        {
            x.summed_size = 0;
            for (auto &v : x)
            {
                x.summed_size += v.iov_len;
                buffer buf(v.iov_base, v.iov_len);
                d()->process_field(buf);
            }
        }

        // return total package //**Important**//
        uint32_t init_data_iov(int maxDataPerPkt)
        {
            IOVector _iov;
            uint32_t &data_index = total_pkg;
            data_index = 0;
            int temp = maxDataPerPkt;
            for (int i = 0; i < iov.iovcnt(); ++i)
            {
                if (iov[i].iov_len <= 0) // 比如空的string
                    continue;
                iovec t_iov = iov[i];
                while (t_iov.iov_len > 0)
                {
                    if (temp == maxDataPerPkt)
                    {                                                 // a new package;
                        data_pkg_index[data_index++] = _iov.iovcnt(); // record a new package;
                    }
                    if (temp > t_iov.iov_len)
                    {
                        // ERPC_INFO("1temp = %d, iov[i].iov_base = 0x%X, iov[i].iov_len = %d\n",temp ,t_iov.iov_base ,t_iov.iov_len);
                        _iov.push_back(t_iov.iov_base, t_iov.iov_len);
                        temp -= t_iov.iov_len;
                        t_iov.iov_base = static_cast<uint8_t *>(t_iov.iov_base) + t_iov.iov_len;
                        t_iov.iov_len = 0;
                    }
                    else if (temp < t_iov.iov_len)
                    {
                        // ERPC_INFO("2temp = %d, t_iov[i].iov_base = 0x%X, t_iov[i].iov_len = %d\n",temp ,t_iov.iov_base ,t_iov.iov_len);
                        _iov.push_back(t_iov.iov_base, temp);
                        t_iov.iov_base = static_cast<uint8_t *>(t_iov.iov_base) + temp;
                        t_iov.iov_len -= temp;
                        temp = maxDataPerPkt;
                    }
                    else if (temp == t_iov.iov_len)
                    {
                        // ERPC_INFO("3temp = %d, t_iov[i].iov_base = 0x%X, t_iov[i].iov_len = %d\n",temp ,t_iov.iov_base ,t_iov.iov_len);
                        _iov.push_back(t_iov.iov_base, t_iov.iov_len);
                        temp = maxDataPerPkt;
                        t_iov.iov_base = static_cast<uint8_t *>(t_iov.iov_base) + t_iov.iov_len;
                        t_iov.iov_len = 0;
                    }
                }
            }
            data_pkg_index[data_index] = _iov.iovcnt(); // 加一个虚拟的包作为最后，实际上没有这个长度。避免0，2，2这种情况；变成0，2，3。
            // ERPC_INFO("there is data_index(%d) packages and %d iovs\n", data_index, _iov.iovcnt());
            this->iov.iov_begin = this->iov.iov_end = data_index;
            for (int i = 0; i < _iov.iovcnt(); ++i)
            {
                this->iov.push_back(_iov[i]);
            }
            return data_index; // total pkg.
        }

        template <typename T>
        void serialize(T &x)
        {
            static_assert(
                std::is_base_of<Message, T>::value,
                "only Messages are permitted");
            // PRINTF((uint64_t)&x);
            // serialize aligned fields, non-aligned fields, and the main body
            auto aligned = FilterAlignedFields(this, true);
            x.process_fields(aligned);
            auto non_aligned = FilterAlignedFields(this, false);
            x.process_fields(non_aligned);
            buffer msg(&x, sizeof(x));
            d()->process_field(msg);
        }
    };

    class DeserializerIOV : public ArchiveBase<DeserializerIOV>
    {
    public:
        iovector *_iov;
        bool failed = false;

        using ArchiveBase<DeserializerIOV>::process_field;

        void process_field(buffer &x)
        {
            if (x.size() == 0)
                return;
            // PRINT("x.size() = ");
            // PRINTF(x.size());
            x._ptr = _iov->extract_front_continuous(x.size());
            if (!x._ptr)
                failed = true;
        }

        void process_field(iovec_array &x)
        {
            iovector_view v;
            ssize_t ret = _iov->extract_front(x.summed_size, &v);
            if (ret == (ssize_t)x.summed_size)
            {
                x.assign(v.iov, v.iovcnt);
            }
            else
            {
                failed = true;
            }
        }

        template <typename T>
        T *deserialize(iovector *iov)
        {
            static_assert(
                std::is_base_of<Message, T>::value,
                "only Messages are permitted");

            // deserialize the main body from back
            _iov = iov;
            auto t = iov->extract_back<T>();
            if (t)
            {
                // deserialize aligned fields, and non-aligned fields, from front
                auto aligned = FilterAlignedFields(this, true);
                t->process_fields(aligned);
                auto non_aligned = FilterAlignedFields(this, false);
                t->process_fields(non_aligned);
            }
            else
            {
                failed = true;
            }
            return t;
        }
    };
}
