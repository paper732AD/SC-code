#include "../../../serialize.h"
#include "Node.h"
#include <chrono>
#include <ctime>
#include <gflags/gflags.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include "../util/time_cycle.h"

using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

Greeter::SayHelloAll::Request req;
extern  void ser_func (SerializerIOV& ser, Greeter::SayHelloAll::Request& req);
extern  void block_invoke_ser (SerializerIOV& ser, Greeter::SayHelloAll::Request& req);
/**
测试 single string field 
*/
int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    PRINTF("\n*******************************huibuffer_ser_time begin")
    // 对象大小
    std::cout << "size of struct is " << sizeof(req) << std::endl;
    // 序列化反序列化的次数
    int N = FLAGS_times;

    // 创建一个std::string
    auto str=std::string(FLAGS_total_len,'h');
    // // 开始时间点
    // auto start = std::chrono::steady_clock::now();
    
    // 这个是指针的地址
    // std::cout << static_cast<void*>(&str) << std::endl;
    // // 这个是元素首地址
    // std::cout << static_cast<const void*>(str.c_str()) << std::endl;
    // // 这个是结构体对象的地址
    // std::cout << static_cast<void*>(&req) << std::endl;
    unsigned long long tsc0;
    unsigned long long tsc1;
    unsigned long long diff = 0;
    req.a.assign(str); //这一步就是为req对象的字段赋值
    for (int i=0; i<N ;i++) {
        SerializerIOV ser;
        tsc0 = get_cpu_cycle();
        // 创建序列化对象.最终的结构
        ser.serialize(req);
        tsc1 = get_cpu_cycle();
        diff += tsc1 - tsc0;
        
    }
    auto time1 = (double)(diff)/N;


    std::cout << "ser time = " << time1 << "cycles" << std::endl;
    return 0;
}
