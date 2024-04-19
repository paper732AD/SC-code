#include <time.h>
#include "test.pb.h"
#include <string.h>
#include <string>
#include <cstddef>
#include <gflags/gflags.h>
#include <cstring>
using namespace std;
string str;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");
DEFINE_uint32(split_len, 1000, "the length of array\n");

void ser_func (test & temp){
    temp.SerializeToString(&str);
}

void  dser_func (test &resp){
    char* _str;
    _str = new char[str.size()+1];
    for(int i=0; i*FLAGS_split_len < str.size(); i++){
        memcpy(_str+i*FLAGS_split_len, str.c_str()+i*FLAGS_split_len, min((int)FLAGS_split_len, int(str.size()) - i*(int)FLAGS_split_len));
    }
    _str[str.size()] = 0;
    resp.ParseFromString(string(_str));
}


int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);    
    
    int N=FLAGS_times;
    int len=FLAGS_total_len;

    test temp,resp;

    std::string str="";
    for(int i=0;i<len/2;++i)
        str+="as";

    temp.set_a(str);
    cout << "size of struct = " << sizeof(temp) << endl;;

    auto st = std::chrono::steady_clock::now();
    for (int i=0;i<N;i++){
        ser_func(temp);
    }
    auto ed = std::chrono::steady_clock::now();
    auto sp = ed - st;
    cout << "after serialization, size = " << str.size() << endl;
    /* serialize time */
    cout << "per serialize time is ";
    cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    cout << " ns"<<endl;


    st = std::chrono::steady_clock::now();
    for (int i=0;i<N;i++){
        dser_func(resp);
    }
    ed = std::chrono::steady_clock::now();
    sp = ed - st;
    /* deserialize time */
    cout << "per deserialize time is ";
    cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    cout << " ns"<<endl;

    return 0;
}