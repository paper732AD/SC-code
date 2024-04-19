#include "../../../serialize.h"
#include "Node.h"
#include <chrono>
#include <gflags/gflags.h>
#include "../util/time_cycle.h"



using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(item_len, -1, "item size in array\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

Greeter_array::SayHelloAll_array::Request_array req;



extern  void ser_func (SerializerIOV& ser, Greeter_array::SayHelloAll_array::Request_array& req);
extern  void block_invoke_ser (SerializerIOV& ser, Greeter_array::SayHelloAll_array::Request_array& req);

int main(int argc, char**argv) {
    // 总长度，每个字段的长度
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    if(FLAGS_item_len==-1){
        PRINTF("Please identify the length of item_len.");
    }
    req = get_req_array(FLAGS_total_len, FLAGS_item_len);
    // PRINTF((uint64_t)&req);
    SerializerIOV _ser;
    _ser.serialize(req);
    PRINT("_ser.iov.sum() = ");
    PRINTF(_ser.iov.sum());
    PRINT("_ser.iov.iovcnt() is ");
    PRINTF(_ser.iov.iovcnt());
    PRINT("_ser.iov[0] is ");
    PRINTF(_ser.iov.begin()->iov_len);

    PRINTF("\n*******************************huibuffer_ser_time begin")
    std::cout << "size of struct is " << sizeof(req) << std::endl;
    int N = FLAGS_times;
    auto start = get_cpu_cycle();
    for (int i=0; i<N ;i++) {
        SerializerIOV ser;
        ser_func(ser,req);
        if(!i){
            PRINT(ser.iov.iovs[0].iov_len);
        }
    }
    auto end = get_cpu_cycle();
    auto sp1 = end - start;

    PRINT("ser time  = ")
    auto t1 = sp1;
    PRINT(t1*1.0/N);
    PRINTF(" cycles")



    // start = get_cpu_cycle();
    // for (int i=0; i<N ;i++){
    //     SerializerIOV ser;
    //     block_invoke_ser(ser, req);
    // }
    // end = get_cpu_cycle();
    // auto sp2 = end - start;
    // PRINT("repeat/alloc/invoke time = ")
    // auto t2 = sp2;
    // PRINT(t2);
    // PRINTF(" cycles")

    // PRINT("total ser time = ")
    // PRINT(t1-t2);
    // PRINTF(" cycles")

    // PRINT("repeat time = ")
    // PRINTF(N);

    // PRINT("result: per ser time = ")
    // PRINT((t1-t2)*1.0/N);
    // PRINTF(" cycles")

    // PRINTF("*******************************huibuffer_ser_time end\n")

    return 0;
}
