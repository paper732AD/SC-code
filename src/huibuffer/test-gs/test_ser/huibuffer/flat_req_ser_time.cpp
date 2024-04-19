#include <chrono>
#include <gflags/gflags.h>

#include "../../../serialize.h"
#include "flatLogFlushChunkRequestPB.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

request req;


extern  void ser_func (SerializerIOV& ser, request& req);
extern  void block_invoke_ser (SerializerIOV& ser, request& req);


int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);

    req = get_flat_req(FLAGS_total_len);
    // SerializerIOV ser;
    // ser_func(ser,req);
    // for(int i = 0 ; i < ser.iov.iovcnt() ; i ++){
    //     std::cout << ser.iov[i].iov_base << " " << ser.iov[i].iov_len << "\n";
    // }
    PRINTF("\n*******************************huibuffer_ser_time begin")

    std::cout << "size of struct is " << sizeof(req) << std::endl;
    int N = FLAGS_times;
    auto start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++) {
        SerializerIOV ser;
        ser_func(ser,req);
    }
    auto end = std::chrono::steady_clock::now();
    auto sp1 = end - start;

    PRINT("repeat/alloc/invoke/ser time  = ")
    auto t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp1).count();
    PRINT(t1);
    PRINTF(" ns")

    SerializerIOV _ser;
    ser_func(_ser,req);
    PRINT("size of _ser = ");
    PRINTF(_ser.iov.sum());

    start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        block_invoke_ser(ser, req);
    }
    end = std::chrono::steady_clock::now();
    auto sp2 = end - start;
    PRINT("repeat/alloc/invoke time = ")
    auto t2 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp2).count();
    PRINT(t2);
    PRINTF(" ns")

    PRINT("total ser time = ")
    PRINT(t1-t2);
    PRINTF(" ns")

    PRINT("repeat time = ")
    PRINTF(N);

    PRINT("result: per ser time = ")
    PRINT((t1-t2)*1.0/N);
    PRINTF(" ns")

    PRINTF("*******************************huibuffer_ser_time end\n")

    return 0;
}
