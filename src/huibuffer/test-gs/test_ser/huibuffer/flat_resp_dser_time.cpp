#include <chrono>
#include <gflags/gflags.h>

#include "../../../serialize.h"
#include "flatLogFlushChunkRequestPB.h"
using namespace RPC;

DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");

response req;
response* resp;

void dser_func (SerializerIOV& ser, DeserializerIOV& dser, response*& req);
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, response*& req);

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
    req = get_flat_resp(FLAGS_total_len);
    std::cout << sizeof(req.trace_data) << std::endl;
    std::cout << req.trace_data.length() << std::endl;
    std::cout << &req << std::endl;
    std::cout << &req.Errorcode << "\n" << &req.ChunkStatus << "\n" << &req.Hint << "\n";
    std::cout << &req.AckLength << "\n" << &req.trace_type << "\n" << &req.trace_data << "\n";
    std::cout << &req.Handle << "\n" << &req.StorageId << "\n" << &req.DiskId << "\n";
    std::cout << &req.RequestId << "\n" << &req.Checksum << "\n" << &req.LeftReplicaSize << "\n";    PRINTF("\n*******************************huibuffer_dser_time begin")
    SerializerIOV ser;
        SerializerIOV ser_new;
    ser.serialize(req);
    char *buffer = new char[10000];
    uint64_t s = 0;
    for(int i=0; i<ser.iov.iovcnt(); ++i){
        std::cout << ser.iov[i].iov_base << " " << ser.iov[i].iov_len << std::endl;
            ser_new.iov.push_back(ser.iov[i].iov_base,ser.iov[i].iov_len);
        if(i==1)memcpy(buffer+s,ser.iov[i].iov_base,ser.iov[i].iov_len);
        s = s +ser.iov[i].iov_len;
        // PRINT(ser.iov[i].iov_len);
        // PRINT(" ");
    }
    auto c = reinterpret_cast<response*>(buffer+1000);
    std::cout << &c << std::endl;
    // std::cout << sizeof(response) << std::endl;
    std::cout << c->trace_data << std::endl;
    // std::cout << "----------------\n";
    // std::cout << s << std::endl;
    // std::cout<<buffer << std::endl;
    //     // PRINTF(" ");
    // // SerializerIOV ser_new;
    // // ser_new.iov.push_back()
    // DeserializerIOV dser;

    // dser_func(ser_new, dser, resp);
    //     std::cout << resp->Hint << std::endl;

    // for(int i=0; i<ser.iov.iovcnt(); ++i){
    //     std::cut<<d
    // }
    // std::cout << resp->trace_data << std::endl;
    // int N = FLAGS_times;
    // auto start = std::chrono::steady_clock::now();
    // for (int i=0; i<N ;i++){
    //     SerializerIOV ser;
    //     ser.serialize(req);
    //     // for(int i=0; i<ser.iov.iovcnt(); ++i){
    //     //     PRINT(ser.iov[i].iov_len);
    //     //     PRINT(" ");
    //     // }
    //     // PRINTF(" ");
    //     DeserializerIOV dser;
    //     dser_func(ser, dser, resp);
    // }
    // auto end = std::chrono::steady_clock::now();
    // auto sp1 = end - start;
    // PRINT("repeat/alloc/ser/invoke/dser time  = ")
    // auto t1 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp1).count();
    // PRINT(t1);
    // PRINTF(" ns")

    // SerializerIOV _ser;
    // _ser.serialize(req);
    // PRINT("sum of _ser = ");
    // PRINTF(_ser.iov.sum());
    // PRINT("_ser.iov.iovcnt() is ");
    // PRINTF(_ser.iov.iovcnt());
    // PRINT("_ser.iov[0] is ");
    // PRINTF(_ser.iov.begin()->iov_len);
    
    // start = std::chrono::steady_clock::now();
    // for (int i=0; i<N ;i++){
    //     SerializerIOV ser;
    //     ser.serialize(req);
    //     DeserializerIOV dser;
    //     block_invoke_dser(ser, dser, resp);
    // }
    // end = std::chrono::steady_clock::now();
    // auto sp2 = end - start;
    // PRINT("repeat/alloc/ser/invoke time = ")
    // auto t2 = std::chrono::duration_cast<std::chrono::nanoseconds>(sp2).count();
    // PRINT(t2);
    // PRINTF(" ns")

    // PRINT("total dser time = ")
    // PRINT(t1-t2);
    // PRINTF(" ns")

    // PRINT("repeat time = ")
    // PRINTF(N);

    // PRINT("result: per dser time = ")
    // PRINT((t1-t2)*1.0/N);
    // PRINTF(" ns")

    // PRINTF("*******************************huibuffer_dser_time end\n")

    return 0;
}
