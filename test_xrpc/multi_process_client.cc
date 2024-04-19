#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <gflags/gflags.h>
#include <thread>
#include <fstream>
#include <iostream>

#include "../hello_world/common.h"
#include "../src/huibuffer/flat_log.h"
#include "../src/huibuffer/serialization.h"
#include "../src/rpc.h"
#include "../src/util/timer.h"
#include "../src/req_handle.h"
using namespace erpc;

DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-102410\n");
DEFINE_uint32(request, 10000, "the number of request\n");
DEFINE_bool(record_time, false, "the number of request\n");
DEFINE_uint32(processes, 1, "the number of processes to run client\n");

Rpc<CTransport> *rpc;
request *req;
ReplicaLocationPB *replicaLocationPB;
void *resp;
std::string str, host_name, userid;
int session_num;
int status;
std::ofstream outfile;
tmpoint st;
std::ofstream outfile_detail;
int No;

erpc::Serializer reqser, respser;
erpc::Deserializer deserializer;

Buffer _vstring, _vbuf, _vhdr, _vhost_nem, _vuserid_mem, v_rep;
pkthdr_t *_hdr;

void cont_func(void *sslot, void *)
{
  SSlot *_sslot = (SSlot *)sslot;
  ERPC_INFO("iov.iovcnt()=%d, iov.sum()=%d\n", _sslot->dyn_resp_msgbuf_.iov.iovcnt(), _sslot->dyn_resp_msgbuf_.iov.sum());
  *(_sslot->client_info_.resp_msgbuf_) = _sslot->dyn_deserializer.deserialize<response>(&(_sslot->dyn_resp_msgbuf_.iov));
  latency.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count - 1])).count()));

  response *_resp = static_cast<response *>(resp);
  ERPC_INFO("in cont_func _resp.a = %d\n", _resp->a);
  ERPC_INFO("in cont_func _resp.trace_data = %s\n", _resp->trace_data.c_str());

  ERPC_INFO("hello ?!\n");
  if (_resp->a < FLAGS_request)
  {
    reqser.clear();
    req->a = lat_client_count;
    lat_client[lat_client_count++] = NOW();
    reqser.serialize(*req);
    ERPC_INFO("After serialize, lat_client_count-1 = %d\n", lat_client_count - 1);
    if (record_time_flag)
      after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count - 1])).count()));
    ERPC_INFO("After after_serialize.push_back\n");
    reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
    ERPC_INFO("After reqser.init_data_iov\n");
    if (record_time_flag)
      after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count - 1])).count()));
    ERPC_INFO("After after_init.push_back\n");
    rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func,
                         nullptr, uint32_t(_vbuf.lkey_), _hdr);
  }
  else
  {
    auto duration =
        std::chrono::duration_cast<Nanoseconds>(NOW() - st);
    double total_time = double(duration.count()); // Nanosecond.
    int total_request = _resp->a + 1;

    outfile_detail.open("latency_test_detail.txt");
    write_detail_to_file(outfile_detail, latency);

    outfile << No << " process is complete." << std::endl;
    // out_serialize(outfile, total_request);
    // out_init_time(outfile, total_request);
    // out_first_send(outfile, total_request);
    // out_before_send(outfile, total_request);
    // out_first_receive(outfile, total_request);
    // out_at_receive(outfile, total_request);
    output_latency(outfile, total_request);

    outfile << "each request size is " << reqser.iov.sum() << " B." << std::endl;
    outfile << "total time is " << total_time / 1000000000 << " s." << std::endl;
    outfile << "total request is " << total_request << "." << std::endl;
    outfile << "QPS is " << total_request / total_time * 1000000000 << std::endl;
    outfile << "Bandwidth(MB/s) is " << 2 * (reqser.iov.sum()) * _resp->a / total_time * 1000000000 / 1024 / 1024 << std::endl;

    outfile.close();
    outfile_detail.close();
    // exit(0);
  }
}

void sm_handler(int, erpc::SmEventType, erpc::SmErrType, void *) {}

int main(int argc, char **argv)
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  if (FLAGS_record_time == false)
  {
    record_time_flag = false;
  }
  else
  {
    record_time_flag = true;
  }

  No = 0;
  pid_t pid;
  uint32_t sub_process = 1; // fork times = 8-1
  while (sub_process++ < FLAGS_processes)
  {
    pid = fork();
    if (pid == 0)
    {
      kUDPPort_c = kUDPPort_c + sub_process;
      break;
    }
    else if (pid < 0)
    {
      ERPC_ERROR("fork failed at %u.\n", sub_process);
    }
    No++;
  }

  char filename[15];
  sprintf(filename, "record%d.log", No);
  outfile.open(filename);

  std::string client_uri = kClientHostname + ":" + std::to_string(kUDPPort_c);
  erpc::Nexus nexus(client_uri);

  rpc = new erpc::Rpc<erpc::CTransport>(&nexus, nullptr, 0, sm_handler);

  std::string server_uri = kServerHostname + ":" + std::to_string(kUDPPort_s);
  session_num = rpc->create_session(server_uri, 0);
  while (!rpc->is_connected(session_num))
    rpc->run_event_loop_once();

  // init req
  _vbuf = rpc->alloc_buffer(sizeof(request));
  req = new (_vbuf.buf_) request();
  _vstring = rpc->alloc_buffer(sizeof(char) * 1124100);
  _vhost_nem = rpc->alloc_buffer(sizeof(char) * 30);
  _vuserid_mem = rpc->alloc_buffer(sizeof(char) * 40);
  v_rep = rpc->alloc_buffer(sizeof(ReplicaLocationPB));

  req->a = 123123123;
  str = std::string((FLAGS_data_len - 495) / 3, 'h');
  host_name = "https://192.168.0.201:10889";
  userid = "4f062dd5-05e5-48e3-821e-35e46ca028ce";
  sprintf((char *)_vstring.buf_, str.c_str());
  sprintf((char *)_vhost_nem.buf_, host_name.c_str());
  sprintf((char *)_vuserid_mem.buf_, userid.c_str());
  req->Data.assign((const char *)(_vstring.buf_));
  req->HostName.assign((const char *)(_vhost_nem.buf_));
  req->UserId.assign((const char *)(_vuserid_mem.buf_));
  req->OpenChunkOptions.ChunkOptions.Placement.assign((const char *)(_vstring.buf_));

  replicaLocationPB = new (v_rep.buf_) ReplicaLocationPB();
  replicaLocationPB->CSLocation.assign((const char *)(_vstring.buf_));
  req->OpenChunkOptions.ChunkLocation.assign(replicaLocationPB, 1);
  req->Strategy.Strategies._len = 0;

  // attempt to serialize
  reqser.serialize(*req);
  uint32_t total_pkg = reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  _vhdr = rpc->alloc_buffer(sizeof(pkthdr_t) * (total_pkg));
  _hdr = new (_vhdr.buf_) pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;
  ERPC_INFO("_vhdr.lkey_ = %u\n", _vhdr.lkey_);
  ERPC_INFO("_vbuf.lkey_ = %u\n", _vbuf.lkey_);
  ERPC_INFO("_vstring.lkey_ = %u\n", _vstring.lkey_);
  ERPC_INFO("address req is 0x%X\n", &req->a);
  ERPC_INFO("address _hdr is 0x%X\n", &_hdr);
  ERPC_WARN("the size of message sent is %d\n", reqser.iov.sum());

  // init time data
  lat_client_count = 0;
  lat_client = std::vector<tmpoint>(FLAGS_request + 1, NOW());
  latency.reserve(FLAGS_request + 1);
  after_serialize.reserve(FLAGS_request + 1);
  after_init.reserve(FLAGS_request + 1);
  before_send.reserve(FLAGS_request + 1);
  at_receive.reserve(FLAGS_request + 1);
  first_receive.reserve(FLAGS_request + 1);
  first_send.reserve(FLAGS_request + 1);
  first_expl_cr.reserve(FLAGS_request + 1);
  last_expl_cr.reserve(FLAGS_request + 1);
  reqser.clear();

  // let's go
  st = NOW();
  req->a = lat_client_count;
  lat_client[lat_client_count++] = NOW();

  reqser.serialize(*req);
  if (record_time_flag)
    after_serialize.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count - 1])).count()));

  reqser.init_data_iov(erpc::CTransport::kMaxDataPerPkt);
  if (record_time_flag)
    after_init.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count - 1])).count()));

  rpc->enqueue_request(session_num, kReqType, &reqser, &resp, cont_func, nullptr, uint32_t(_vbuf.lkey_), _hdr);

  rpc->run_event_loop(1000000);

  rpc->free_msg_buffer(_vbuf);
  rpc->free_msg_buffer(_vhdr);
  rpc->free_msg_buffer(_vstring);
  delete rpc;

  // output_latency();
  wait(&status);
  exit(0);
}
