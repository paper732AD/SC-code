#include "../../../serialize.h"
#include "Node.h"
#include <chrono>
#include <gflags/gflags.h>
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");
DEFINE_uint32(split_len, 1000, "the length of array\n");

Greeter::SayHelloAll::Request req;
Greeter::SayHelloAll::Response* resp;

void dser_func (SerializerIOV& ser, DeserializerIOV& dser, Greeter::SayHelloAll::Response*& resp);
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, Greeter::SayHelloAll::Response*& resp);

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    req = get_req(FLAGS_total_len);
    resp = get_resp();

    PRINTF("\n*******************************huibuffer_dser_time begin")

    int N = FLAGS_times;
    auto start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        ser.init_data_iov(FLAGS_split_len);
        DeserializerIOV dser;
        dser_func(ser, dser, resp);
    }
    auto end = std::chrono::steady_clock::now();
    auto sp1 = end - start;
    PRINT("repeat/alloc/ser/invoke/dser time  = ")
    auto t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp1).count();
    PRINT(t1);
    PRINTF(" ns")

    // PRINT("resp->a = ");
    // PRINTF(resp->a);

    start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        ser.init_data_iov(FLAGS_split_len);
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
