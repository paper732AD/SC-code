#include <time.h>
#include "test.pb.h"
#include <string.h>
#include <string>
#include <cstddef>
#include <gflags/gflags.h>
using namespace std;
typedef std::chrono::nanoseconds Nanoseconds;

string str;
uint8_t* _str;
int _size;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

void ser_func (test & temp){
    temp.SerializeToString(&str);
    // temp.SerializeToArray(_str, temp.ByteSizeLong());
}

void  dser_func (test &resp){
    resp.ParseFromString(str);
    // resp.ParseFromArray(_str, _size);
}
double _avglat(std::vector<double>& temp) {
    double sum = 0;
    for (auto lat : temp) {
        sum += lat;
    }
    return sum / temp.size();
}

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);    int N=FLAGS_times;
    int len=FLAGS_total_len;

    test temp,resp;

    _str = new uint8_t[1100000];
    std::string str="";
    for(int i=0;i<len/2;++i)
        str+="as";

    temp.set_a(str);
    cout << "size of struct = " << sizeof(temp) << endl;;
    _size = temp.ByteSizeLong();
    std::vector<double> latency_ser;
    std::vector<double> latency_dser;
    // auto st = std::chrono::steady_clock::now();
    for (int i=0;i<N;i++){
        auto st = std::chrono::steady_clock::now();
        ser_func(temp);
        auto ed = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();
        latency_ser.push_back(double(duration));
        st = std::chrono::steady_clock::now();
        dser_func(resp);
        ed = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();
        latency_dser.push_back(double(duration));
    }
        cout << "per serialize time is ";
    cout << _avglat(latency_ser);
    cout << " ns"<<endl;
    cout << "per deserialize time is ";
    cout << _avglat(latency_dser);
    cout << " ns"<<endl;
    // auto ed = std::chrono::steady_clock::now();
    // auto sp = ed - st;
    // cout << "after serialization, size = " << str.size() << endl;
    // /* serialize time */
    // cout << "per serialize time is ";
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    // cout << " ns"<<endl;


    // st = std::chrono::steady_clock::now();
    // for (int i=0;i<N;i++){
    //     dser_func(resp);
    // }
    // // std::cout << resp.a().c_str() <<endl;
    // ed = std::chrono::steady_clock::now();
    // sp = ed - st;
    // /* deserialize time */
    // cout << "per deserialize time is ";
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    // cout << " ns"<<endl;

    return 0;
}