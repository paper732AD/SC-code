#include <time.h>
#include "flat_generated.h"
#include <string.h>
#include <string>
#include <cstddef>
#include <chrono>
#include <gflags/gflags.h>
#include <iostream>
using namespace std;
typedef std::chrono::nanoseconds Nanoseconds;

flatbuffers::FlatBufferBuilder builder;
static string temp;
int data_size;
DEFINE_uint32(times, 1000, "times serialization or deserialization\n");
DEFINE_uint32(total_len, 1000, "the length of array\n");
// uint8_t* serialize(int FLAGS_data_len,int& size);
void deserialize(uint8_t *ser_data);
// uint8_t* d;
double _avglat(std::vector<double>& temp) {
    double sum = 0;
    for (auto lat : temp) {
        sum += lat;
    }
    return sum / temp.size();
}
uint8_t* serialize(int FLAGS_data_len,int& size)
{
    // flatbuffers::FlatBufferBuilder builder;
    // flatbuffers::FlatBufferBuilder builder(FLAGS_data_len+100);
    // builder.Clear();
    temp = std::string((FLAGS_data_len-656)/3, 'h');
    auto flat_temp1 = builder.CreateString(temp);
    auto flat_temp2 = builder.CreateString(temp);
    auto flat_temp3 = builder.CreateString(temp);
    // set ChunkIdTypeCJFB 
    auto chunkIdTypeCJFB = CreateChunkIdTypeCJFB(builder, 123123, 123123, 123123);

    // set IONiceFB

    auto ioNiceFB = CreateIONiceFB(builder, 123123);

    // set MirrorArgsFB

    auto mirrorArgsFB = CreateMirrorArgsFB(builder, 123123, 123123);

    // set ChunkOptionsFB

    auto chunkOptionFB = CreateChunkOptionsFB(builder, 123123, mirrorArgsFB, flat_temp1);

    // set ReplicaLocationFB

    auto replicaLocationFB = CreateReplicaLocationFB(builder, flat_temp2, 123123, 123123);

    std::vector<flatbuffers::Offset<ReplicaLocationFB>> offset_replicaLocationFB;
    offset_replicaLocationFB.push_back(replicaLocationFB);
    auto vector_replicaLocationFB = builder.CreateVector(offset_replicaLocationFB);

    // set AccelerateOpenOptionsFB

    auto accelerateOpenOptionsFB = CreateAccelerateOpenOptionsFB(builder, chunkOptionFB,
                                                                 vector_replicaLocationFB, 123123, 123123);

    // set PanguTraceSpanInfo

    auto panguTraceSpanInfoFB = CreatePanguTraceSpanInfoFB(builder, 123123, 123123, 123123);

    // set CsChooseDiskStrategySingleCJFB

    auto csChooseDiskStrategySingleCJFB = CreateCsChooseDiskStrategySingleCJFB(builder, 123123, 123123);

    std::vector<flatbuffers::Offset<CsChooseDiskStrategySingleCJFB>> offset_csChooseDiskStrategySingleCJFB;
    offset_csChooseDiskStrategySingleCJFB.push_back(csChooseDiskStrategySingleCJFB);

    auto vector_csChooseDiskStrategySingleCJFB = builder.CreateVector(offset_csChooseDiskStrategySingleCJFB);

    // set CsChooseDiskStrategyCJFB
    auto csChooseDiskStrategyCJFB = CreateCsChooseDiskStrategyCJFB(builder, vector_csChooseDiskStrategySingleCJFB, 123123, 123123);

    // set UUIDInfoFB
    auto uuIDInfoFB = CreateUUIDInfoFB(builder, 123123, 123123);
    auto uuIDInfoFB2 = uuIDInfoFB;
    auto uuIDInfoFB3 = uuIDInfoFB;

    // set ChunkOrStreamIdInfoFB
    auto chunkOrStreamIdInfoFB = CreateChunkOrStreamIdInfoFB(builder, uuIDInfoFB, 123123, 1231231);

    // set QosOptionsFB

    auto qosOptionsFB = CreateQosOptionsFB(builder, 123123, 123123, 123123, 123123);

    // set FlushChecksumFB
    auto flushChecksumFB = CreateFlushChecksumFB(builder, 123123, 123123);

    // set RootServerIdFB
    auto rootServerIdFB = CreateRootServerIdFB(builder, uuIDInfoFB2, 123123);

    // set hostname and userid
    std::string hostname = "https://192.168.0.201:10889";
    std::string user_id = "4f062dd5-05e5-48e3-821e-35e46ca028ce";

    // std:: cout << hostname.length() + user_id.length() << std:: endl;
    // auto flat_hostname = builder.CreateVector(reinterpret_cast<const unsigned char*>(hostname.c_str()), hostname.length());
    // auto flat_user_id = builder.CreateVector(reinterpret_cast<const unsigned char*>(user_id.c_str()), user_id.length());
    auto flat_hostname = builder.CreateString(hostname);
    auto flat_user_id = builder.CreateString(user_id);
    // in the end , set rquest  存在内存对齐问题,待优化。。。。。。

    FlatLogFileFlushChunkRequestFBBuilder request_builder(builder);

    request_builder.add_chunk_id(chunkIdTypeCJFB);
    request_builder.add_offset(123123);
    request_builder.add_length(123123);
    request_builder.add_storage_id(123123);
    request_builder.add_data(flat_temp3);
    request_builder.add_hint(123123);
    request_builder.add_is_replication(false);
    request_builder.add_trace_open(false);
    request_builder.add_io_nice(ioNiceFB);
    request_builder.add_open_chunkoptions(accelerateOpenOptionsFB);
    request_builder.add_open_mode(123123);
    request_builder.add_handle(123123);
    request_builder.add_data_sync(false);
    request_builder.add_trace_span(panguTraceSpanInfoFB);
    request_builder.add_create_ifnotexists(false);
    request_builder.add_strategy(csChooseDiskStrategyCJFB);
    request_builder.add_chunk_id_v2(chunkOrStreamIdInfoFB);
    request_builder.add_qos_options(qosOptionsFB);
    request_builder.add_checksum(flushChecksumFB);
    request_builder.add_volume_uuid(uuIDInfoFB3);
    request_builder.add_module_version(123123123);

    request_builder.add_hostname(flat_hostname);
    request_builder.add_disk_id(123123);
    request_builder.add_request_id(123123);
    request_builder.add_user_id(flat_user_id);
    request_builder.add_root_server_id(rootServerIdFB);
    request_builder.add_required_min_csversion(123123);
    request_builder.add_fromtype(123123);
    request_builder.add_offset64(123123);
    request_builder.add_record_size(123123);
    request_builder.add_pool_id(123123);
    request_builder.add_storage_pool_confversion(123123);
    request_builder.add_a(123123);
    auto request = request_builder.Finish();
    builder.Finish(request);
    size = builder.GetSize();
    // std::cout << size << std::endl;
    // memcpy(dest,builder.GetBufferPointer(),size);
    return builder.GetBufferPointer();
}
int main(int argc,char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);    
    int N=FLAGS_times;
    int len=FLAGS_total_len;
    std::vector<double> latency_ser;
    std::vector<double> latency_dser;
    uint8_t* d = new uint8_t[1100000];
    for (int i=0;i<N;i++){
        auto st = std::chrono::steady_clock::now();
        serialize(len,data_size);

        auto ed = std::chrono::steady_clock::now();
        builder.Clear();

        auto duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();
        latency_ser.push_back(double(duration));
        memset(d,0,sizeof d);
        auto now = serialize(len,data_size);
        memcpy(d,now,data_size);
        st = std::chrono::steady_clock::now();
        deserialize(d);

        ed = std::chrono::steady_clock::now();
        builder.Clear();

        duration = std::chrono::duration_cast<Nanoseconds>(ed-st).count();
        latency_dser.push_back(double(duration));
    }
    std::cout << data_size << std::endl;
    cout << "per serialize time is ";
    cout << _avglat(latency_ser);
    cout << " ns"<<endl;
    cout << "per deserialize time is ";
    cout << _avglat(latency_dser);
    cout << " ns"<<endl;
    // for(auto x : latency_dser){
    //     cout << x << endl;
    // }
    // auto sp = ed - st;
    // auto ser_data = serialize(len,data_size);
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
    // cout << chrono::duration_cast<std::chrono::nanoseconds>(sp).count()/N;
    // cout << " ns"<<endl;

    return 0;
}

void deserialize(uint8_t* ser_data)
{
    auto request = flatbuffers::GetRoot<FlatLogFileFlushChunkRequestFB>(ser_data);
    // std::string s = request->data()->c_str();
    // std::cout << s.back() << std::endl;
    // std::cout << request->data()->c_str() << std::endl;
}