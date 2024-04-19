#include "../hello_world/common.h"
#include "stdio.h"
#include <atomic>
#include <gflags/gflags.h>
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-10240\n");
DEFINE_bool(record_time, false, "the number of request\n");

erpc::Rpc<erpc::CTransport> *rpc;
erpc::Serializer respser;
ctrl_buffer_array *resp, *req;
pkthdr_t* _hdr;
std::atomic<int> count;

Buffer _vbuf,_vstring,_vhdr;
uint32_t  total_pkg;
std::string data;

void init_Buffer (){
    // allocate memory from hugepages
    _vbuf = rpc->alloc_buffer(sizeof(ctrl_buffer_array));
    _vstring = rpc->alloc_buffer(sizeof(char)*102410);
      
    resp = new(_vbuf.buf_) ctrl_buffer_array();
    resp->a = 123123;
    constexpr size_t _kMaxDataPerPkt = erpc::CTransport::kMaxDataPerPkt;
    int iovec_num =  FLAGS_data_len/_kMaxDataPerPkt + (FLAGS_data_len%_kMaxDataPerPkt!=0?1:0);
    data = std::string(_kMaxDataPerPkt,'h');
    data[data.size()-1] = '\0';
    sprintf((char *)_vstring.buf_, data.c_str());

    iovec* iov_array = new iovec[iovec_num];
    for(int i=0,j=0;i<FLAGS_data_len;i+=_kMaxDataPerPkt,j++){
      (iov_array+j)->iov_base = _vstring.buf_;
      (iov_array+j)->iov_len = std::min(_kMaxDataPerPkt, FLAGS_data_len-j*_kMaxDataPerPkt);
      ERPC_INFO("(iov_array+j)->iov_len = %d pushed\n",(iov_array+j)->iov_len);
    }
    resp->hahaha.assign(iov_array, iovec_num);

    respser.serialize(*resp);
    total_pkg = respser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
    _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t)*total_pkg);
    _hdr = new(_vhdr.buf_) pkthdr_t[total_pkg];
    _hdr->total_pkt = total_pkg;
}

void req_handler(erpc::ReqHandle *req_handle, void* _t) {
  req_handle->server_info_.req_msgbuf_ = (void*)ctrl_buffer_array::deserialize(&req_handle->dyn_resp_msgbuf_.iov);
  ctrl_buffer_array* req = static_cast<ctrl_buffer_array*>(req_handle->get_req_msgbuf());
  ERPC_INFO("req->a = %d\n", req->a);
  ERPC_INFO("req->hahaha[0].len = %d\n", req->hahaha.begin()->iov_len);
  ERPC_INFO("req->hahaha[0] = %s\n", req->hahaha.begin()->iov_base);
  respser.clear();
  resp->a = req->a;
  respser.serialize(*resp);
  respser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  // TODO 这里就只是放普通的请求回复
  rpc->enqueue_response(req_handle, &respser, uint32_t(_vbuf.lkey_), _hdr);

  // Deserializer dser;
  // void *t_resp = dser.deserialize<ctrl_buffer_array>(&respser.iov);
  // ERPC_INFO("deserialize finished, address of resp is 0x%X\n",t_resp);
  // ctrl_buffer_array* _resp = static_cast<ctrl_buffer_array*>(t_resp);
  // ERPC_INFO("address of _resp is 0x%X\n",_resp);
  // ERPC_INFO("_resp.a = %d\n",_resp->a);
  // ERPC_INFO("_resp.a = %d, _resp.hahaha = %s\n",_resp->a, _resp->hahaha.begin()->iov_base);
}

int main(int argc, char**argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if(FLAGS_record_time == false){
    record_time_flag = false;
  }else{
    record_time_flag = true;
  }
  
  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  erpc::Nexus nexus(server_uri);
  nexus.register_req_func(kReqType, req_handler);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, nullptr);
  ERPC_WARN("sizeof(pkthdr_t)=%d\n",sizeof(pkthdr_t));
  
  init_Buffer();

  rpc->run_event_loop(10000000);

  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);

  delete rpc;
}
