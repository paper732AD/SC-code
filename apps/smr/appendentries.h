/**
 * @file appendentries.h
 * @brief Handlers for appendentries RPC
 */

#pragma once
#include "smr.h"

// With eRPC, there is currently no way for an RPC server to access connection
// data for a request, so the client's Raft node ID is included in the request.
// 重新定义这个结构体，并重新定义序列化serialize和反序列化unpack方法
struct app_appendentries_t {
  int node_id;  // Node ID of the sender
  msg_appendentries_t msg_ae;
  // If ae.n_entries > 0, the msg_entry_t structs are serialized here. Each
  // msg_entry_t struct's buf is placed immediately after the struct.

  // Serialize the ingredients of an app_appendentries_t into a network buffer
  static void serialize(raft_req_tag_t* &rtt, int node_id,
                        msg_appendentries_t *msg_ae) {
    uint8_t *buf = rtt->req.buf_;
    app_appendentries_t *srlz = reinterpret_cast<app_appendentries_t *>(buf);

    // app_appendentries_t *srlz = static_cast<app_appendentries_t*>(rtt->req);
    // Copy the whole-message header
    srlz->node_id = node_id;
    srlz->msg_ae = *msg_ae;
    srlz->msg_ae.entries = nullptr;  // Was local pointer
    buf += sizeof(app_appendentries_t);

    // Serialize each entry in the message
    // 下看看有几条命令，每条需要同步的东西有两部分：
    // 这里把数据都存到了 rtt->req.buf_中，长度是调用这个函数时候记录的，应该是
    // “msg_entry_t 任期、索引序号、类型”；“client_req_t KV”
    for (size_t i = 0; i < static_cast<size_t>(msg_ae->n_entries); i++) {
      // Copy the entry header
      *reinterpret_cast<msg_entry_t *>(buf) = msg_ae->entries[i];
      reinterpret_cast<msg_entry_t *>(buf)->data.buf = nullptr;  // Local ptr
      buf += sizeof(msg_entry_t);

      // Copy the entry data
      assert(msg_ae->entries[i].data.len == sizeof(client_req_t));
      memcpy(buf, msg_ae->entries[i].data.buf, sizeof(client_req_t));
      buf += sizeof(client_req_t);
    }

    assert(buf == req_msgbuf.buf_ + req_msgbuf.get_data_size());

  }

  static constexpr size_t kStaticMsgEntryArrSize = 16;

  // Unpack an appendentries request message received at the server.
  //  * The buffers for entries the unpacked message come from the mempool.
  //  * The entries array for the unpacked message is dynamically allocated
  //    if there are too many entries. Caller must free if so.
  static void unpack(erpc::ReqHandle *req_handle,
                     msg_entry_t *static_msg_entry_arr,
                     AppMemPool<client_req_t> &log_entry_appdata_pool) {
    erpc::Serializer* req_msgbuf = &req_handle->dyn_resp_msgbuf_;
    
    uint8_t* _vbuf = req_handle->pre_req;
    uint8_t* buf = req_handle->pre_req;
    for(int i=0; i<req_msgbuf->iov.iovcnt(); ++i){
      memcpy(_vbuf, req_msgbuf->iov[i].iov_base, req_msgbuf->iov[i].iov_len);
      _vbuf += req_msgbuf->iov[i].iov_len;
    }

    auto *ae_req = reinterpret_cast<app_appendentries_t *>(buf);
    msg_appendentries_t &msg_ae = ae_req->msg_ae;
    assert(msg_ae.entries == nullptr);

    size_t n_entries = static_cast<size_t>(msg_ae.n_entries);
    bool is_keepalive = (n_entries == 0);

    if (!is_keepalive) {
      // Non-keepalive appendentries requests contain app-defined log entries
      buf += sizeof(app_appendentries_t);
      msg_ae.entries = n_entries <= kStaticMsgEntryArrSize
                           ? static_msg_entry_arr
                           : new msg_entry_t[n_entries];

      // Invariant: buf points to a msg_entry_t, followed by its buffer
      for (size_t i = 0; i < n_entries; i++) {
        msg_ae.entries[i] = *(reinterpret_cast<msg_entry_t *>(buf));
        buf += sizeof(msg_entry_t);

        assert(msg_ae.entries[i].data.buf == nullptr);
        msg_ae.entries[i].data.buf = log_entry_appdata_pool.alloc();

        // Copy out each SMR command buffer from the request msgbuf since the
        // msgbuf is valid for this function only.
        assert(msg_ae.entries[i].data.len == sizeof(client_req_t));
        memcpy(msg_ae.entries[i].data.buf, buf, sizeof(client_req_t));
        buf += sizeof(client_req_t);
      }

      assert(buf == req_msgbuf->buf_ + req_msgbuf->get_data_size());
    }
  }
};

// appendentries request format is like so:
// node ID, msg_appendentries_t, [{size, buf}]
void appendentries_handler(erpc::ReqHandle *req_handle, void *_context) {
  ERPC_INFO("In appendentries_handler\n");
  auto *c = static_cast<AppContext *>(_context);
  // req_handle->server_info_.req_msgbuf_ = (void*)req_handle->dyn_deserializer.deserialize<app_appendentries_t>(&req_handle->dyn_resp_msgbuf_.iov);
  // auto req = static_cast<app_appendentries_t*>(req_handle->get_req_msgbuf());
  // const erpc::MsgBuffer *req_msgbuf = req_handle->get_req_msgbuf();

  if (kAppTimeEnt) c->server.time_ents.emplace_back(TimeEntType::kRecvAeReq);

  // Reconstruct an app_appendentries_t in req_msgbuf. The entry buffers the
  // unpacked message are long-lived (pool-allocated). The unpacker may choose
  // to not use static_msg_entry_arr for the unpacked entries, in which case
  // we free the dynamic memory later below.
  msg_entry_t static_msg_entry_arr[app_appendentries_t::kStaticMsgEntryArrSize];

  erpc::Buffer _buf = c->rpc->alloc_buffer(req_handle->dyn_resp_msgbuf_.iov.sum());
  req_handle->pre_req = _buf.buf_;
  // 这里unpack请求？
  app_appendentries_t::unpack(req_handle, static_msg_entry_arr,
                              c->server.log_entry_appdata_pool);

  // 这里拿到请求
  auto *ae_req = reinterpret_cast<app_appendentries_t *>(req_handle->pre_req);
  msg_appendentries_t &msg_ae = ae_req->msg_ae;

  if (kAppVerbose) {
    printf("smr: Received appendentries (%s) req from node %s [%s].\n",
           msg_ae.n_entries == 0 ? "keepalive" : "non-keepalive",
           node_id_to_name_map[ae_req->node_id].c_str(),
           erpc::get_formatted_time().c_str());
  }

  erpc::Buffer pre_resp = c->rpc->alloc_buffer(sizeof(msg_appendentries_response_t));

  // Only the buffers for entries in the append
  int e = raft_recv_appendentries(
      c->server.raft, raft_get_node(c->server.raft, ae_req->node_id), &msg_ae,
      reinterpret_cast<msg_appendentries_response_t *>(pre_resp.buf_));
  erpc::rt_assert(e == 0, "raft_recv_appendentries failed");

  if (msg_ae.entries != static_msg_entry_arr) delete[] msg_ae.entries;

  if (kAppTimeEnt) c->server.time_ents.emplace_back(TimeEntType::kSendAeResp);

  req_handle->pre_resp_msgbuf_.clear();
  req_handle->pre_resp_msgbuf_.iov.push_back(pre_resp.buf_, sizeof(msg_appendentries_response_t));
  int total_pkt = req_handle->pre_resp_msgbuf_.init_data_iov(erpc::IBTransport::kMaxDataPerPkt);
  erpc::Buffer pre_hdr = c->rpc->alloc_buffer(sizeof(erpc::pkthdr_t)*total_pkt);
  erpc::pkthdr_t* _pre_hdr = new(pre_hdr.buf_) erpc::pkthdr_t[total_pkt];
  _pre_hdr->total_pkt = 1; 
  _pre_hdr->lkey = pre_hdr.lkey_; 

  // important
  c->rpc->enqueue_response(req_handle, &req_handle->pre_resp_msgbuf_, pre_resp.lkey_, _pre_hdr, c);
}

void appendentries_cont(void *, void *);  // Fwd decl

// Raft callback for sending appendentries message
static int smr_raft_send_appendentries_cb(raft_server_t *, void *,
                                          raft_node_t *node,
                                          msg_appendentries_t *msg_ae) {
  ERPC_INFO("In smr_raft_send_appendentries_cb()\n");
  auto *conn = static_cast<connection_t *>(raft_node_get_udata(node));
  AppContext *c = conn->c;

  bool is_keepalive = (msg_ae->n_entries == 0);
  if (kAppVerbose) {
    printf("smr: Sending appendentries (%s) to node %s [%s].\n",
           is_keepalive ? "keepalive" : "non-keepalive",
           node_id_to_name_map[raft_node_get_id(node)].c_str(),
           erpc::get_formatted_time().c_str());
  }

  if (!c->rpc->is_connected(conn->session_num)) {
    if (kAppVerbose) {
      printf("smr: Cannot send ae req on session %d.\n", conn->session_num);
    }
    return 0;
  }

  // Compute the request size. Keepalive appendentries requests do not have
  // a buffer, but they have an unused msg_entry_t (???).
  size_t req_size = sizeof(app_appendentries_t);
  for (size_t i = 0; i < static_cast<size_t>(msg_ae->n_entries); i++) {
    assert(msg_ae->entries[i].data.len == sizeof(client_req_t));
    req_size += sizeof(msg_entry_t) + sizeof(client_req_t);
  }

  erpc::rt_assert(req_size <= c->rpc->get_max_msg_size(),
                  "send_appendentries_cb: Message size too large");

  raft_req_tag_t *rrt = c->server.raft_req_tag_pool.alloc();
  rrt->req = c->rpc->alloc_buffer(req_size);
  rrt->node = node;

  // 把msg_ae的数据填充到 rrt->req._buf 中
  app_appendentries_t::serialize(rrt, c->server.node_id, msg_ae);
  rrt->req_msgbuf.clear();
  rrt->req_msgbuf.iov.push_back(rrt->req.buf_, req_size);
  uint32_t hdr_num = rrt->req_msgbuf.init_data_iov(erpc::IBTransport::kMaxDataPerPkt);
  erpc::Buffer _vhdr = c->rpc->alloc_buffer( sizeof(erpc::pkthdr_t) * hdr_num );
  erpc::pkthdr_t* _hdr = new(_vhdr.buf_) erpc::pkthdr_t[hdr_num];
  _hdr->lkey = _vhdr.lkey_;
  _hdr->total_pkt = hdr_num;

  if (kAppTimeEnt) c->server.time_ents.emplace_back(TimeEntType::kSendAeReq);

  // important
  ERPC_INFO("In smr_raft_send_appendentries_cb, before enqueue_request,"
            "hdr_num = %d, req_size = %d, begin and end = %d and %d\n", hdr_num, req_size,
             rrt->req_msgbuf.iov.iov_begin, rrt->req_msgbuf.iov.iov_end);
  c->rpc->enqueue_request(conn->session_num,
                          static_cast<uint8_t>(ReqType::kAppendEntries),
                          &rrt->req_msgbuf, &rrt->resp_msgbuf,
                          appendentries_cont, reinterpret_cast<void *>(rrt),
                          _hdr->lkey, _hdr, c);
  return 0;
}

void appendentries_cont(void *_sslot, void *_tag) {
  ERPC_INFO("In appendentries_cont\n");
  erpc::SSlot *sslot = static_cast<erpc::SSlot *>(_sslot);
  AppContext *c = static_cast<AppContext *>(sslot->c);

  if (kAppTimeEnt) c->server.time_ents.emplace_back(TimeEntType::kRecvAeResp);
  auto *rrt = reinterpret_cast<raft_req_tag_t *>(_tag);

  erpc::Serializer* resp_msgbuf = static_cast<erpc::Serializer*>(rrt->resp_msgbuf);
  if (likely( resp_msgbuf->iov.sum() > 0)) {
  // if (likely(rrt->resp_msgbuf.get_data_size() > 0)) {
    // The RPC was successful
    if (kAppVerbose) {
      printf("smr: Received appendentries response from node %s [%s].\n",
             node_id_to_name_map[raft_node_get_id(rrt->node)].c_str(),
             erpc::get_formatted_time().c_str());
    }

    int e = raft_recv_appendentries_response(
        c->server.raft, rrt->node,
        reinterpret_cast<msg_appendentries_response_t *>(
            resp_msgbuf->iov[0].iov_base));
    erpc::rt_assert(e == 0 || e == RAFT_ERR_NOT_LEADER,
                    "raft_recv_appendentries_response error");
  } else {
    // The RPC failed. Fall through and call raft_periodic() again.
    printf("smr: Appendentries RPC to node %s failed to complete [%s].\n",
           node_id_to_name_map[raft_node_get_id(rrt->node)].c_str(),
           erpc::get_formatted_time().c_str());
  }

  // c->rpc->free_msg_buffer(rrt->req_msgbuf);
  // c->rpc->free_msg_buffer(rrt->resp_msgbuf);
  c->server.raft_req_tag_pool.free(rrt);
}
