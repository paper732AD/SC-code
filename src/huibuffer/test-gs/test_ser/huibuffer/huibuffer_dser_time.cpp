#include "../../../serialize.h"
#include "Node.h"
#include <chrono>
#include <gflags/gflags.h>
#include "../util/time_cycle.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

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
    unsigned long long total = 0;
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        DeserializerIOV dser;
        auto start = get_cpu_cycle();
        resp = dser.deserialize<Greeter::SayHelloAll::Response>(&ser.iov);
        auto end = get_cpu_cycle();
        total += end - start;
    }
    std:: cout << "dser cycles: " << (double)(total)/N << std::endl;
    PRINTF("*******************************huibuffer_dser_time end\n")

    return 0;
}
