#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <gflags/gflags.h>
#include <thread>
#include <fstream>
#include <iostream>

#include "../hello_world/common.h"
#include "../src/huibuffer/ctrl_buffer.h"
#include "../src/huibuffer/serialization.h"
#include "../src/rpc.h"
#include "../src/util/timer.h"
#include "../src/req_handle.h"
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-102410\n");
DEFINE_uint32(request, 10000, "the number of request\n");
DEFINE_uint32(first_split, 952, "测试scatter-gather使用，不进行这个测试的时候请注释掉这一行\n");
DEFINE_bool(record_time, false, "the number of request\n");

Rpc<CTransport> *rpc;
ctrl_buffer *req;
void *resp;
std::string data;
int session_num;
int status;
std::ofstream outfile;
std::ofstream outfile_detail;
tmpoint st;

erpc::Serializer reqser, respser;
erpc::Deserializer deserializer;

Buffer _vbuf;
Buffer _vstring;
Buffer _vhdr;
pkthdr_t *_hdr;
int one, multi;

void cont_func(void *sslot, void *) {
  SSlot* _sslot = (SSlot*)sslot;
  // if(_sslot->dyn_resp_msgbuf_.iov.iovcnt() == 1){
  //   one++;
  // }else{
  //   multi++;
  // }
  // ERPC_WARN("iov.iovcnt()=%d, iov.sum()=%d\n",_sslot->dyn_resp_msgbuf_.iov.iovcnt(), _sslot->dyn_resp_msgbuf_.iov.sum());
  *(_sslot->client_info_.resp_msgbuf_) = _sslot->dyn_deserializer.deserialize<ctrl_buffer>(&(_sslot->dyn_resp_msgbuf_.iov));
  latency.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));

  ctrl_buffer* _resp = static_cast<ctrl_buffer *>(resp);
  ERPC_INFO("in cont_func _resp.a = %d\n", _resp->a);
  ERPC_INFO("in cont_func _resp.hahaha = %s\n", _resp->hahaha.c_str());



  ERPC_INFO("hello ?!\n");
  if(_resp->a < FLAGS_request){
      reqser.clear();
      req->a = lat_client_count;
      reqser.serialize(*req);
      ERPC_INFO("After serialize, lat_client_count-1 = %d\n", lat_client_count-1);
      if(record_time_flag) after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
      ERPC_INFO("After after_serialize.push_back\n");
      reqser.init_data_iov(FLAGS_first_split);
      reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
      ERPC_INFO("After reqser.init_data_iov\n");
      // after_init[lat_client_count-1] = double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
      if(record_time_flag) after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
      ERPC_INFO("After after_init.push_back\n");
      lat_client[lat_client_count++] = NOW();
      rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func,
                        nullptr, uint32_t(_vbuf.lkey_), _hdr);
  }else{
    auto duration = 
      std::chrono::duration_cast<Nanoseconds>(NOW() - st);
    double total_time = double(duration.count()); // Nanosecond.
    int total_request = _resp->a+1;

    outfile_detail.open("latency_test_detail.txt");
    write_detail_to_file(outfile_detail, latency);
    
    out_serialize( outfile,total_request);
    out_init_time( outfile,total_request);
    out_first_send( outfile,total_request);
    // out_first_expl_cr( outfile,total_request);
    // out_last_expl_cr( outfile,total_request);
    out_before_send( outfile,total_request);
    out_first_receive( outfile,total_request);
    out_at_receive( outfile,total_request);
    output_latency( outfile,total_request);

    outfile << "each request size is " << reqser.iov.sum() << " B." << std::endl;
    outfile << "total time is " << total_time/1000000000 << " s." << std::endl;
    outfile << "total request is " << total_request << "." << std::endl;
    outfile << "QPS is " << total_request/total_time*1000000000 << std::endl;
    outfile << "Bandwidth(MB/s) is " << 2*(reqser.iov.sum())*_resp->a/total_time*1000000000/1024/1024 << std::endl;

    outfile.close();
    outfile_detail.close();
    // ERPC_WARN("one = %d, multi = %d\n", one, multi);
    exit(0);
  }

}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main(int argc, char**argv) {
	gflags::ParseCommandLineFlags(&argc, &argv, true);
  if(FLAGS_record_time == false){
    record_time_flag = false;
  }else{
    record_time_flag = true;
  }
  outfile.open("latency_test.txt");

    one=0;
    multi=0;

  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort_c);
  erpc::Nexus nexus(client_uri);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  // 这里客户端给server发送了一个udp包请求链接
  session_num = rpc->create_session(server_uri, 0);

  // * 这里状态应该是 inprocess。 然后所以一直通过 run_event_loop_once
  // 检测状态，直到连上了为止
  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer));
  _vstring = rpc->alloc_buffer(sizeof(char) * 1124100);

  req = new (_vbuf.buf_) ctrl_buffer();
  req->a = 123123123;
  data = std::string(FLAGS_data_len,'h');
  sprintf((char *)_vstring.buf_, data.c_str());
  req->hahaha.assign((const char *)(_vstring.buf_));

  reqser.serialize(*req);
  reqser.init_data_iov(FLAGS_first_split);
  uint32_t total_pkg = reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t) * (total_pkg));
  ERPC_INFO("_vhdr.lkey_ = %u\n", _vhdr.lkey_);
  ERPC_INFO("_vbuf.lkey_ = %u\n", _vbuf.lkey_);
  ERPC_INFO("_vstring.lkey_ = %u\n", _vstring.lkey_);
  _hdr = new (_vhdr.buf_) pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;

  ERPC_INFO("address req is 0x%X\n", &req->a);
  ERPC_INFO("address _hdr is 0x%X\n", &_hdr);
  ERPC_WARN("total_pkg = %d\n", total_pkg);


  lat_client_count = 0;
  lat_client=std::vector<tmpoint>(FLAGS_request+1, NOW());
  latency.reserve(FLAGS_request+1);
  after_serialize.reserve(FLAGS_request+1);
  after_init.reserve(FLAGS_request+1);
  before_send.reserve(FLAGS_request+1);
  at_receive.reserve(FLAGS_request+1);
  first_receive.reserve(FLAGS_request+1);
  first_send.reserve(FLAGS_request+1);
  first_expl_cr.reserve(FLAGS_request+1);
  last_expl_cr.reserve(FLAGS_request+1);

  st = NOW();
  ERPC_WARN("the size of message sent is %d\n", reqser.iov.sum());

  reqser.clear();
  req->a = lat_client_count;

  reqser.serialize(*req);
  if(record_time_flag) after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  
  reqser.init_data_iov(FLAGS_first_split);
  reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  // after_init[lat_client_count-1] = double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  if(record_time_flag) after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  lat_client[lat_client_count++] = NOW();

  rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func,
                        nullptr, uint32_t(_vbuf.lkey_), _hdr);

  rpc->run_event_loop(1000000);

  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);
  delete rpc;
  
  // output_latency();
  wait(&status);
  exit(0);
}