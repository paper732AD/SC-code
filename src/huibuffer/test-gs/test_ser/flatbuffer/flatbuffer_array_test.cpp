#include <time.h>
#include "array_generated.h"
#include <string.h>
#include <string>
#include <cstddef>
#include <chrono>
#include <gflags/gflags.h>
#include <iostream>
using namespace std;
typedef std::chrono::nanoseconds Nanoseconds;

// flatbuffers::FlatBufferBuilder builder;
static string temp;
int data_size;
flatbuffers::FlatBufferBuilder builder;
DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");
uint8_t* serialize(std::vector<uint8_t> data,int& size,int data_len);
void deserialize(uint8_t *ser_data);
static std::vector<uint8_t> temp_v;
double _avglat(std::vector<double>& temp) {
    double sum = 0;
    for (auto lat : temp) {
        sum += lat;
    }
    return sum / temp.size();
}

int main(int argc,char **argv)
{

    gflags::ParseCommandLineFlags(&argc, &argv, true);    
    int N=FLAGS_times;
    int len=FLAGS_total_len;
    // char a[3] = "as";
    // string s = "";
    // for(int i = 0; i<len ; i++){
    //     s+=a[i%2];
    // }
    std::cout << len << std::endl;
    std::string s = std::string(FLAGS_total_len,'h');
    for(int i = 0 ; i < len; i++){
        temp_v.emplace_back('a');
        // temp_v.emplace_back('s');
        // temp+="as";
    }

    std::vector<double> latency_ser;
    std::vector<double> latency_dser;
    for (int i=0;i<N;i++){
        auto st = std::chrono::steady_clock::now();
        serialize(temp_v,data_size,len);
        builder.Clear();
        auto ed = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();
        latency_ser.push_back(double(duration));
        // builder.Reset();
        auto ser_data = serialize(temp_v,data_size,len);
        st = std::chrono::steady_clock::now();
        deserialize(ser_data);
        builder.Clear();

        ed = std::chrono::steady_clock::now();
        duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();

        latency_dser.push_back(double(duration));
                // builder.Reset();

        // serialize(temp_v,data_size,len);
        // builder.Clear();
        // builder.Reset();
    }
     cout << "per serialize time is ";
    cout << _avglat(latency_ser);
    cout << " ns"<<endl;
    cout << "per deserialize time is ";
    cout << _avglat(latency_dser);
    cout << " ns"<<endl;


    // auto ed = std::chrono::steady_clock::now();
    // auto sp = ed - st;
    // auto ser_data = serialize(temp_v,data_size,len);
    // cout << "data_size = " << data_size << endl;
    // /* serialize time */
    // cout << "per serialize time is ";
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    // cout << " ns"<<endl;


    // st = std::chrono::steady_clock::now();
    // for (int i=0;i<N;i++){
    //     deserialize(ser_data);
    // }
    // ed = std::chrono::steady_clock::now();
    // sp = ed - st;
    // /* deserialize time */
    // cout << "per deserialize time is ";
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count() << endl;
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/(double)N;
    // cout << " ns"<<endl;

    return 0;
}
uint8_t* serialize(std::vector<uint8_t> data,int& size,int data_len)
{
    // builder.Clear();
    // flatbuffers::FlatBufferBuilder builder(data_len+100);
    auto data_v = builder.CreateVector(data);
    auto Msg = Createmsg(builder,data_v);
    builder.Finish(Msg);
    size = builder.GetSize();
    // memcpy(dest,builder.GetBufferPointer(),size);
    // // auto data_s = builder.CreateString(data);
    // msgBuilder msg_builder(builder);
    // msg_builder.add_data(data_s);
    // auto msg_f = msg_builder.Finish();
    // // auto Msg = Createmsg(builder,data_s);
    // builder.Finish(msg_f);
    // size = builder.GetSize();
    return builder.GetBufferPointer();
}
void deserialize(uint8_t* ser_data)
{
    auto msg_datra = flatbuffers::GetRoot<msg>(ser_data);
}