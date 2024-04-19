#include <chrono>
#include <gflags/gflags.h>

#include "../../../serialize.h"
#include "flatLogFlushChunkRequestPB.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

request req;
request* resp;

void dser_func (SerializerIOV& ser, DeserializerIOV& dser, request*& req);
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, request*& req);

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    req = get_flat_req(FLAGS_total_len);

    PRINTF("\n*******************************huibuffer_dser_time begin")

    int N = FLAGS_times;
    auto start = std::chrono::steady_clock::now();
    for (int i=0; i<N ;i++){
        SerializerIOV ser;
        ser.serialize(req);
        // for(int i=0; i<ser.iov.iovcnt(); ++i){
        //     PRINT(ser.iov[i].iov_len);
        //     PRINT(" ");
        // }
        // PRINTF(" ");
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
    PRINTF(resp->Checksum.value);
    PRINTF(resp->HostName.c_str());
    // PRINTF(resp->OpenChunkOptions.ChunkOptions.Placement.c_str());
    
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
