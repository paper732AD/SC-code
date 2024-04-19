#include "common.h"
#include "../src/huibuffer/serialization.h"
#include "../src/huibuffer/ctrl_buffer.h"
#include "../src/rpc.h"
#include "stdio.h"

using namespace erpc;

Rpc<erpc::CTransport> *rpc;
ctrl_buffer* req;
void* resp;

void cont_func(void *, void *) { 
  ERPC_INFO("in cont_func resp.hahaha = %s\n", static_cast<ctrl_buffer*>(resp)->hahaha);
  printf("hello ?!\n"); 
}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main() {
  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort_c);
  erpc::Nexus nexus(client_uri);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  // 这里客户端给server发送了一个udp包请求链接
  int session_num = rpc->create_session(server_uri, 0);
  
  // * 这里状态应该是 inprocess。 然后所以一直通过 run_event_loop_once 检测状态，直到连上了为止
  while (!rpc->is_connected(session_num)) rpc->run_event_loop_once();
  erpc::Serializer reqser,respser;
  erpc::Deserializer deserializer;

  Buffer _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer));
  Buffer _vstring = rpc->alloc_buffer(sizeof(char)*4500);


  // TODO 这里就只是放普通的请求回复
  // resp = rpc->hui.get_ctrl_buffer();
  // TODO 这里就只是放普通的请求
  req = new(_vbuf.buf_) ctrl_buffer();
  req->a = 123123123;
  // sprintf((char*)_vstring.buf_, "helloworld\0");
  sprintf((char*)_vstring.buf_, "clientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhello22clientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhello22\0");
  req->hahaha.assign((const char*)(_vstring.buf_));
  // req->hahaha2.assign((const char*)(_vstring.buf_));

  reqser.serialize(*req);
  uint32_t total_pkg  = reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  Buffer _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t)*(total_pkg));
  ERPC_INFO("_vhdr.lkey_ = %u\n",_vhdr.lkey_);
  ERPC_INFO("_vbuf.lkey_ = %u\n",_vbuf.lkey_);
  ERPC_INFO("_vstring.lkey_ = %u\n",_vstring.lkey_);
  pkthdr_t* _hdr = new(_vhdr.buf_) pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;

  ERPC_INFO("address req is 0x%X\n", &req->a);
  ERPC_INFO("address _hdr is 0x%X\n", &_hdr);


  rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func, nullptr, uint32_t(_vbuf.lkey_), _hdr);
  rpc->run_event_loop(100);


  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);
  delete rpc;
}
