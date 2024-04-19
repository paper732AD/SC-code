#include <chrono>
#include <gflags/gflags.h>
#include "../../../serialize.h"
#include "Node.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(item_len, -1, "item size in array\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

Greeter_array::SayHelloAll_array::Request_array req;
Greeter_array::SayHelloAll_array::Response_array* resp;

void dser_func (SerializerIOV& ser, DeserializerIOV& dser, Greeter_array::SayHelloAll_array::Response_array*& resp);
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser,Greeter_array::SayHelloAll_array::Response_array*& resp);

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    if(FLAGS_item_len==-1){
        PRINTF("Please identify the length of item_len.");
    }
    req = get_req_array(FLAGS_total_len, FLAGS_item_len);
    resp = get_resp_array();

    PRINTF("\n*******************************huibuffer_dser_time begin")

    int N = FLAGS_times;
    auto start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        DeserializerIOV dser;
        dser_func(ser, dser, resp);
    }
    auto end = std::chrono::steady_clock::now();
    auto sp1 = end - start;
    PRINT("repeat/alloc/ser/invoke/dser time  = ")
    auto t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp1).count();
    PRINT(t1);
    PRINTF(" ns")

    SerializerIOV _ser;
    _ser.serialize(req);
    PRINT("size of _ser = ");
    PRINTF(_ser.iov.sum());
    PRINT("_ser.iov.iovcnt() is ");
    PRINTF(_ser.iov.iovcnt());
    PRINT("_ser.iov[0] is ");
    PRINTF(_ser.iov.begin()->iov_len);



    start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        DeserializerIOV dser;
        block_invoke_dser(ser, dser, resp);
    }
    end = std::chrono::steady_clock::now();
    auto sp2 = end - start;
    PRINT("repeat/alloc/ser/invoke time = ")
    auto t2 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp2).count();
    PRINT(t2);
    PRINTF(" ns")

    PRINT("total dser time = ")
    PRINT(t1-t2);
    PRINTF(" ns")

    PRINT("repeat time = ")
    PRINTF(N);

    PRINT("result: per dser time = ")
    PRINT((t1-t2)*1.0/N);
    PRINTF(" ns")

    PRINTF("*******************************huibuffer_dser_time end\n")

    return 0;
}
