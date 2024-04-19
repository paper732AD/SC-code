/**
 * @file requestvote.h
 * @brief Handlers for requestvote RPC
 */

#pragma once
#include "smr.h"

// With eRPC, there is currently no way for an RPC server to access connection
// data for a request, so the client's Raft node ID is included in the request.
struct app_requestvote_t {
  int node_id;
  msg_requestvote_t msg_rv;
};

// Return a string representation of a requestvote request message
static std::string msg_requestvote_string(msg_requestvote_t *msg_rv) {
  std::ostringstream ret;
  ret << "[candidate_id " << node_id_to_name_map[msg_rv->candidate_id] << ", "
      << "last log idx " << std::to_string(msg_rv->last_log_idx) << ", "
      << "last log term " << std::to_string(msg_rv->last_log_term) << ", "
      << "term " << std::to_string(msg_rv->term) << "]";
  return ret.str();
}

// Return a string representation of a requestvote request message
static std::string msg_requestvote_response_string(
    msg_requestvote_response_t *msg_rv_resp) {
  std::ostringstream ret;
  ret << "[term " << std::to_string(msg_rv_resp->term) << ", "
      << "vote granted " << std::to_string(msg_rv_resp->vote_granted) << "]";
  return ret.str();
}

void requestvote_handler(erpc::ReqHandle *req_handle, void *_context) {
  ERPC_INFO("In requestvote_handler\n");
  auto *c = static_cast<AppContext *>(_context);


  app_requestvote_t* rv_req = (app_requestvote_t *)(req_handle->dyn_resp_msgbuf_.iov[0].iov_base);
  printf("smr: Received requestvote request from Node_id (%d) %s: %s [%s].\n",
         rv_req->node_id,
         node_id_to_name_map[rv_req->node_id].c_str(),
         msg_requestvote_string(&rv_req->msg_rv).c_str(),
         erpc::get_formatted_time().c_str());

  erpc::Serializer &resp_msgbuf = req_handle->pre_resp_msgbuf_;

  erpc::Buffer _vbuf = c->rpc->alloc_buffer(sizeof(msg_requestvote_response_t));
  msg_requestvote_response_t *_resp_buf = new(_vbuf.buf_) msg_requestvote_response_t();

  int e = raft_recv_requestvote(c->server.raft,
                                raft_get_node(c->server.raft, rv_req->node_id),
                                &rv_req->msg_rv, _resp_buf);
  erpc::rt_assert(e == 0, "raft_recv_requestvote failed");

  printf("smr: Sending requestvote response to %s: %s [%s].\n",
         node_id_to_name_map[rv_req->node_id].c_str(),
         msg_requestvote_response_string(_resp_buf).c_str(),
         erpc::get_formatted_time().c_str());

  resp_msgbuf.clear();
  resp_msgbuf.iov.push_back(_resp_buf, sizeof(msg_requestvote_response_t));
  int total_pkg = resp_msgbuf.init_data_iov(erpc::IBTransport::kMaxDataPerPkt);

  erpc::Buffer _vhdr = c->rpc->alloc_buffer(sizeof(erpc::pkthdr_t)*total_pkg);
  erpc::pkthdr_t *_hdr = new(_vhdr.buf_) erpc::pkthdr_t[total_pkg];
  _hdr->total_pkt = total_pkg;
  _hdr->lkey = _vhdr.lkey_;

  // important ~
  c->rpc->enqueue_response(req_handle, &resp_msgbuf, uint32_t(_vbuf.lkey_), _hdr, c);
}

void requestvote_cont(void *, void *);  // Fwd decl

// Raft callback for sending requestvote request
static int smr_raft_send_requestvote_cb(raft_server_t *, void *,
                                        raft_node_t *node,
                                        msg_requestvote_t *msg_rv) {  
  auto *conn = static_cast<connection_t *>(raft_node_get_udata(node));
  AppContext *c = conn->c;

  if (!c->rpc->is_connected(conn->session_num)) {
    printf("smr: Cannot send requestvote request (disconnected).\n");
    return 0;
  }

  printf("smr: Sending requestvote request to node %s [%s].\n",
         node_id_to_name_map[raft_node_get_id(node)].c_str(),
         erpc::get_formatted_time().c_str());

  raft_req_tag_t *rrt = c->server.raft_req_tag_pool.alloc();
  
  rrt->req = c->rpc->alloc_buffer(sizeof(app_requestvote_t));
  app_requestvote_t *rv_req = new(rrt->req.buf_) app_requestvote_t();
  rv_req->node_id = c->server.node_id;
  rv_req->msg_rv = *msg_rv;
  rrt->node = node;

  // 把msg_ae的数据填充到req._buf中
  rrt->req_msgbuf.clear();
  rrt->req_msgbuf.iov.push_back(rv_req, sizeof(app_requestvote_t) );

  uint32_t hdr_num = rrt->req_msgbuf.init_data_iov(erpc::IBTransport::kMaxDataPerPkt);
  erpc::Buffer _vhdr = c->rpc->alloc_buffer( sizeof(erpc::pkthdr_t) * hdr_num );
  erpc::pkthdr_t* _hdr = new(_vhdr.buf_) erpc::pkthdr_t[hdr_num];
  _hdr->lkey = rrt->req.lkey_;
  _hdr->total_pkt = hdr_num;

  // important
  c->rpc->enqueue_request(conn->session_num,
                          static_cast<uint8_t>(ReqType::kRequestVote),
                          &rrt->req_msgbuf, &rrt->resp_msgbuf, requestvote_cont,
                          reinterpret_cast<void *>(rrt),
                          rrt->req.lkey_, _hdr, c);
  return 0;
}

void requestvote_cont(void *__sslot, void *_tag) {
  ERPC_INFO("In requestvote_cont\n");
  erpc::SSlot *_sslot = static_cast<erpc::SSlot *>(__sslot);
  AppContext *c = static_cast<AppContext *>(_sslot->c);

  auto *rrt = reinterpret_cast<raft_req_tag_t *>(_tag);
  erpc::Serializer* resp_msgbuf = static_cast<erpc::Serializer*>(rrt->resp_msgbuf);

  if (likely(resp_msgbuf->iov.sum() > 0)) {
  // if (likely(rrt->resp_msgbuf.get_data_size() > 0)) {
    // The RPC was successful
    auto *msg_rv_resp =
      reinterpret_cast<msg_requestvote_response_t *>(resp_msgbuf->iov[0].iov_base);
      
    printf("smr: gs Received requestvote response from node %s: %s [%s].\n",
           node_id_to_name_map[raft_node_get_id(rrt->node)].c_str(),
           msg_requestvote_response_string(msg_rv_resp).c_str(),
           erpc::get_formatted_time().c_str());

    int e =
        raft_recv_requestvote_response(c->server.raft, rrt->node, msg_rv_resp);
    // XXX: Doc says shutdown if e != 0
    erpc::rt_assert(
        e == 0, "raft_recv_requestvote_response failed");
  } else {
    // This is a continuation-with-failure
    printf("smr: Requestvote RPC to node %s failed to complete [%s].\n",
           node_id_to_name_map[raft_node_get_id(rrt->node)].c_str(),
           erpc::get_formatted_time().c_str());
  }

  // c->rpc->free_msg_buffer(rrt->req_msgbuf);
  // c->rpc->free_msg_buffer(rrt->resp_msgbuf);
  // c->server.raft_req_tag_pool.free(rrt);
}
