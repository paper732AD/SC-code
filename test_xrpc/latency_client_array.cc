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
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-102410\n");
DEFINE_uint32(request, 10000, "the number of request\n");
DEFINE_bool(record_time, false, "the number of request\n");

Rpc<CTransport> *rpc;
ctrl_buffer_array *req;
void *resp;
std::string data;
int session_num;
int status;
std::ofstream outfile;
tmpoint st;

erpc::Serializer reqser, respser;
erpc::Deserializer deserializer;

Buffer _vbuf;
Buffer _vstring;
Buffer _vhdr;
pkthdr_t *_hdr;

void cont_func(void *sslot, void *) {
  SSlot* _sslot = (SSlot*)sslot;
  *(_sslot->client_info_.resp_msgbuf_) = _sslot->dyn_deserializer.deserialize<ctrl_buffer_array>(&(_sslot->dyn_resp_msgbuf_.iov));
  latency.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));

  ctrl_buffer_array* _resp = static_cast<ctrl_buffer_array *>(resp);
  ERPC_INFO("in cont_func _resp.hahaha = %s\n", _resp->hahaha.begin()->iov_base);
  ERPC_INFO("in cont_func _resp.a = %d\n", _resp->a);
  // ERPC_WARN("in cont_func _resp.a = %d\n", _resp->a);

  ERPC_INFO("hello ?!\n");
  if(_resp->a < FLAGS_request){
      reqser.clear();
      req->a = lat_client_count;
      lat_client[lat_client_count++] = NOW();
      reqser.serialize(*req);
      if(record_time_flag)
        after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
      reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
      // after_init[lat_client_count-1] = double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
      if(record_time_flag) after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
      rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func,
                        nullptr, uint32_t(_vbuf.lkey_), _hdr);
  }else{
    auto duration = 
      std::chrono::duration_cast<Nanoseconds>(NOW() - st);
    double total_time = double(duration.count()); // Nanosecond.
    int total_request = _resp->a+1;

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
  outfile.open("latency_test.log");


  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort_c);
  erpc::Nexus nexus(client_uri);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  // 这里客户端给server发送了一个udp包请求链接
  session_num = rpc->create_session(server_uri, 0);

  // * 这里状态应该是 inprocess。 然后所以一直通过 run_event_loop_once
  // 检测状态，直到连上了为止
  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();

  _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer_array));
  _vstring = rpc->alloc_buffer(sizeof(char) * 102410);

  req = new (_vbuf.buf_) ctrl_buffer_array();
  req->a = 123123123;
  constexpr size_t _kMaxDataPerPkt = erpc::CTransport::kMaxDataPerPkt;
  int iovec_num =  FLAGS_data_len/_kMaxDataPerPkt + (FLAGS_data_len%_kMaxDataPerPkt!=0?1:0);
  data = std::string(_kMaxDataPerPkt,'h');
  data[data.size()-1] = '\0';
  sprintf((char *)_vstring.buf_, data.c_str());
  ERPC_INFO("FLAGS_data_len = %d, _kMaxDataPerPkt = %d, iovec_num = %u\n", FLAGS_data_len, _kMaxDataPerPkt, iovec_num);

  iovec* iov_array = new iovec[iovec_num];
  for(int i=0,j=0; i<FLAGS_data_len; i+=_kMaxDataPerPkt,j++){
    (iov_array+j)->iov_base = _vstring.buf_;
    (iov_array+j)->iov_len = std::min(_kMaxDataPerPkt, FLAGS_data_len-(j*_kMaxDataPerPkt));
    ERPC_INFO("(iov_array+j)->iov_len = %d pushed\n",(iov_array+j)->iov_len);
  }
  req->hahaha.assign(iov_array, iovec_num);

  reqser.serialize(*req);
  uint32_t total_pkg = reqser.init_data_iov(_kMaxDataPerPkt);
  _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t) * (total_pkg));
  ERPC_INFO("_vhdr.lkey_ = %u\n", _vhdr.lkey_);
  ERPC_INFO("_vbuf.lkey_ = %u\n", _vbuf.lkey_);
  ERPC_INFO("_vstring.lkey_ = %u\n", _vstring.lkey_);
  _hdr = new (_vhdr.buf_) pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;

  ERPC_INFO("address req is 0x%X\n", &req->a);
  ERPC_INFO("address _hdr is 0x%X\n", &_hdr);

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
  ERPC_WARN("iov_cnt of message sent is %d, the size of message sent is %d\n",reqser.iov.iovcnt(), reqser.iov.sum());

  reqser.clear();
  req->a = lat_client_count;
  lat_client[lat_client_count++] = NOW();

  reqser.serialize(*req);
  if(record_time_flag) after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));

  reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  // after_init[lat_client_count-1] = double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
  if(record_time_flag) after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));

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