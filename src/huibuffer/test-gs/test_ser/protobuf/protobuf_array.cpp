#include <time.h>
#include "test_array.pb.h"
#include <string.h>
#include <string>
#include <cstddef>
#include <gflags/gflags.h>
using namespace std;
string str;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");
DEFINE_uint32(item_len, 1000, "the length of item in array\n");

void ser_func (test & temp){
    temp.SerializeToString(&str);
}

void  dser_func (test &resp){
    resp.ParseFromString(str);
}


int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);    int N=FLAGS_times;
    int len=FLAGS_item_len;

    char a[3] = "as";
    test temp,resp;

    std::string _str="";
    for(int i=0;i<len;++i)
        _str+=a[i%2];

    // cout << str << endl;
    cout << "size of str = " << _str.size() << endl;
    for(int i=0;i<FLAGS_total_len;i+=len){
        temp.add_a(_str);
        // cout << "added a" << endl;
    }
    
    cout << "size of struct = " << temp.ByteSizeLong() << endl;;

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