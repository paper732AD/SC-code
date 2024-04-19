#include "greeter.grpc.fb.h"
#include "greeter_generated.h"

#include <grpcpp/grpcpp.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <gflags/gflags.h>
#include <chrono>
// 请求长度
DEFINE_int32(data_len,1024,"the string length of request");
// 端口号
DEFINE_int32(port,50051,"TCP Port of this server");
// 每个进程的请求次数
DEFINE_uint32(total_request, 1000, "the number of request per processes\n");
// 日志文件
DEFINE_string(log_file, "latency_test.txt", "the number of request per processes\n");
// 输出日志信息
std::ofstream outfile;
std::string temp;
typedef std::chrono::nanoseconds Nanoseconds;
double _50lat(std::vector<double>& temp) { return temp[uint32_t(0.5 * temp.size())]; }
double _90lat(std::vector<double>& temp) { return temp[uint32_t(0.9 * temp.size())]; }
double _95lat(std::vector<double>& temp) { return temp[uint32_t(0.95 * temp.size())]; }
double _99lat(std::vector<double>& temp) { return temp[uint32_t(0.99 * temp.size())]; }
double _avglat(std::vector<double>& temp) {
    double sum = 0;
    for (auto lat : temp) {
        sum += lat;
    }
    return sum / temp.size();
}
int msg_size;
class GreeterClient {
 public:
  GreeterClient(std::shared_ptr<grpc::Channel> channel)
    : stub_(Greeter::NewStub(channel)) {}

  std::string SayHello(int len) {
    // flatbuffers::grpc::MessageBuilder builder;
    temp = std::string(len, 'h');
    // const unsigned char* ucharArray = reinterpret_cast<const unsigned char*>(temp.c_str());
    // auto flat_temp = builder.CreateString(temp);
    // auto flat_temp = builder.CreateVector(ucharArray, FLAGS_data_len);
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
    // auto message_offset = mb.CreateString(message);
    // auto request_offset = CreateHelloRequest(mb, message_offset);
    // mb.Finish(request_offset);
    auto request_msg = builder.ReleaseMessage<FlatLogFileFlushChunkRequestFB>();
    msg_size = request_msg.size();
    flatbuffers::grpc::Message<FlatLogFileFlushChunkResponseFB> response_msg;

    grpc::ClientContext context;

    auto status = stub_->SayHello(&context, request_msg, &response_msg);
    // std::cout << response_msg.size() << std::endl;
    if (status.ok()) {
      // const HelloReply *response = response_msg.GetRoot();
      // return response->message()->str();
      builder.Clear();
      return "ok";
    } else {
      std::cerr << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }
 private:
  flatbuffers::grpc::MessageBuilder builder;

  std::unique_ptr<Greeter::Stub> stub_;
};

int main(int argc, char **argv) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  // std::string server_address("192.168.0.204:50051");
  std::string server_address = "192.168.0.204:" + std::to_string(FLAGS_port);
  auto channel =
      grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials());
  GreeterClient greeter(channel);
  // 写入日志
  outfile.open(FLAGS_log_file+std::to_string(FLAGS_data_len));

  
  // 存储每次请求的时间
  std::vector<double> latency;
  latency.reserve(FLAGS_total_request+1);
  // // totaltime for request 总时间
  // std::chrono::duration<double, std::micro> totalTime(0);
  std::cout << "data_len = " << FLAGS_data_len << "\n";
  std::cout << "port= " << FLAGS_port << "\n";
  for (int i = 0; i < FLAGS_total_request+1; ++i) {
      // 在发送请求前获取当前时间
        auto start = std::chrono::system_clock::now(); // 开始

      greeter.SayHello((FLAGS_data_len-656)/3);

      // 在请求返回后获取当前时间
      // auto end = std::chrono::high_resolution_clock::now();

      // 计算并保存请求的持续时间
      auto end = std::chrono::system_clock::now();
      auto duration = std::chrono::duration_cast<Nanoseconds>(end-start);

      if(i)latency.push_back(double(duration.count()));
  }

//   // std::cout << FLAGS_data_len << std::endl;
//   // 平均时间 = 总时间/请求次数
//   double averageTime = totalTime.count() / FLAGS_total_request;
//   // std::cout << "Average latency for sending a gRPC request: " << averageTime << " us" << std::endl;
//   outfile << "Average latency for sending a gRPC request: " << averageTime << " us" << std::endl;
  
//   // 对持续时间进行排序
//   std::sort(durations.begin(), durations.end());  
//   // 计算并打印50th、90th和99th的延迟
//   // std::cout << "50th latency: " << durations[durations.size() * 0.5] << " us\n";
//   // std::cout << "90th latency: " << durations[durations.size() * 0.9] << " us\n";
//   // std::cout << "95th latency: " << durations[durations.size() * 0.95] << " us\n";
//   // std::cout << "99th latency: " << durations[durations.size() * 0.99] << " us\n";


//   outfile << "50th latency: " << durations[durations.size() * 0.5] << " us\n";
//   outfile << "90th latency: " << durations[durations.size() * 0.9] << " us\n";
//   outfile << "95th latency: " << durations[durations.size() * 0.95] << " us\n";
//   outfile << "99th latency: " << durations[durations.size() * 0.99] << " us\n";


//   // QPS = 总次数/总时间
//   double qps = static_cast<double>(FLAGS_total_request) / (totalTime.count() / 1e6);
//   // std::cout << "QPS is : " << qps << std::endl;
//   outfile << "QPS is : " << qps << std::endl;

//   // Bandwidth = 传输的数据量/总时间

//   double bandwidth = static_cast<double>(FLAGS_total_request) * FLAGS_data_len * 2 / totalTime.count() * 1000000.0 / 1024 / 1024;

//  // std::cout << "BandWidth is : " << bandwidth << " MB/s" << std::endl;
//   outfile << "BandWidth is : " << bandwidth << " MB/s" << std::endl;
    sort(latency.begin(), latency.end());
    
    outfile <<"total records = " << latency.size() << std::endl;
    outfile <<"request size  = " << msg_size << std::endl;
    outfile <<"total latency = " << std::endl;
    outfile <<"\tavge latency = "  << _avglat(latency)/1000 << "    us" << std::endl;
    outfile <<"\t50th latency = " << _50lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t90th latency = " << _90lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t95th latency = " << _95lat(latency)/1000 <<"    us" << std::endl;
    outfile <<"\t99th latency = " << _99lat(latency)/1000 <<"    us" << std::endl;

  outfile.close();

  return 0;
}
