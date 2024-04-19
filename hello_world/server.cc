#include "common.h"
#include "stdio.h"
#include <atomic>
using namespace erpc;
erpc::Rpc<erpc::CTransport> *rpc;
erpc::Serializer respser;
ctrl_buffer *resp, *req;
pkthdr_t* _hdr;
std::atomic<int> count;
void req_handler(erpc::ReqHandle *req_handle, void* _t) {
  //TODO 这里申请并注册内存，
  // erpc::Deserializer deserializer;
  Buffer _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer));
  Buffer _vstring = rpc->alloc_buffer(sizeof(char)*1500);
  
  // req = static_cast<ctrl_buffer * const>(req_handle->get_req_msgbuf());
  // ERPC_INFO("req->a is %d\n",req->a);

  // // TODO 这里就只是放普通的请求
  resp = new(_vbuf.buf_) ctrl_buffer();
  resp->a = 54321;
  sprintf((char*)_vstring.buf_, "clientsayhello\0");
  // sprintf((char*)_vstring.buf_, "clientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhelloclientsayhello\0");
  resp->hahaha.assign((const char*)(_vstring.buf_));
  // resp->hahaha2.assign((const char*)(_vstring.buf_));

  // ERPC_INFO("address resp->a is 0x%X\n", &resp->a);
  respser.clear();
  respser.serialize(*resp);

  auto total_pkg = respser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  Buffer _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t)*total_pkg);
  _hdr = new(_vhdr.buf_) pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;

  // ERPC_INFO("address resp->a is 0x%X\n", &resp->a);
  // ERPC_INFO("value of resp->a is %d\n", resp->a);
  // ERPC_INFO("address resp->hahaha is 0x%X\n", &resp->hahaha);
  // ERPC_INFO("value of resp->hahaha is %s\n", resp->hahaha);
  // ERPC_INFO("address _hdr is 0x%X\n", _hdr);
  // ERPC_INFO("address respser is 0x%X\n", &respser);
  // TODO 这里就只是放普通的请求回复
  // ERPC_INFO("address tx_batch_i_ is 0x%X, value is %u\n", &rpc->tx_batch_i_,rpc->tx_batch_i_);
  rpc->enqueue_response(req_handle, &respser, uint32_t(_vbuf.lkey_), _hdr);
  // rpc->enqueue_response(req_handle, &respser, uint32_t(_vbuf.lkey_), _hdr);

  //～TODO 如何解决，服务端可以回复多条信息的这种情况？
  // soultion：eRPC这种设计不支持服务端主动。客户端发一个，我返回一个。
  
  // rpc->enqueue_response(req_handle, &respser, uint32_t(_vbuf.lkey_), _hdr);
  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);

  // ERPC_WARN("count = %d\n",++count);
}

int main() {
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  erpc::Nexus nexus(server_uri);
  nexus.register_req_func(kReqType, req_handler);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  rpc->run_event_loop(10000000);
}
