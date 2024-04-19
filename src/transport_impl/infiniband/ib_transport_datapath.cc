#ifdef ERPC_INFINIBAND

#include "ib_transport.h"
#include "util/timer.h"

namespace erpc {


// Packets that are the first packet in their MsgBuffer use one DMA, and may
// be inlined. Packets that are not the first packet use two DMAs, and are never
// inlined for simplicity.
void IBTransport::tx_burst(const tx_burst_item_t* tx_burst_arr,
                           size_t num_pkts) {
  bool flag = false;
  bool first_flag = false;
  ERPC_INFO("IBTransport::tx_burst\n");
  for (size_t i = 0; i < num_pkts; i++) {
    // 拿到要发送的数据包
    const tx_burst_item_t& item = tx_burst_arr[i];

    Serializer* msg_buffer = item.msg_buffer_;
    ERPC_INFO("Address of tx_burst_item_t(item) is 0x%X, item.msg_buffer_ is 0x%X ,It is %d iovec in item.msg_buffer_, total_size is %d\n", 
      &item, msg_buffer ,msg_buffer->iov.iovcnt() ,msg_buffer->iov.sum());

    // Verify constant fields of work request
    // TODO 拿到send事件，但是怎么确定这个i没有被重复使用呢？
    struct ibv_send_wr& wr = send_wr[i];
    struct ibv_sge* sgl = send_sgl[i];

    assert(wr.next == &send_wr[i + 1]);  // +1 is valid
    assert(wr.wr.ud.remote_qkey == kQKey);
    // assert(wr.opcode == IBV_WR_SEND_WITH_IMM);
    assert(wr.opcode == IBV_WR_SEND);
    assert(wr.sg_list == sgl);


    // Set signaling + poll SEND CQ if needed. The wr is non-inline by default.
    wr.send_flags = get_signaled_flag() ? IBV_SEND_SIGNALED : 0;

    // TODO 这里肯定是要把我们的包的方式写过来。
    ERPC_INFO("It is %d iov in msg_buffer->iov\n",msg_buffer->iov.iovcnt());
    uint32_t _lkey;
    
    _lkey = static_cast<pkthdr_t*>(msg_buffer->iov.begin()->iov_base)->lkey;
    ERPC_INFO("\tlkey of sgl is %u\n", _lkey);
    ERPC_INFO("\tindex of item is %d,i = %d num_pkts is %d\n",item.pkt_idx_, i , num_pkts);

    // add header
    auto pkthdr = msg_buffer->get_n_hdr(item.pkt_idx_);
    if(pkthdr->is_req()){
      ERPC_INFO("pkthdr->pkt_num_ = %d is ready to be sent.\n", pkthdr->pkt_num_);
    }
    //!为了确定是不是这个message的最后一个包，用来确认是不是要记录时间。
    if(record_time_flag){
      if(!flag && pkthdr->is_req() && pkthdr->total_pkt == item.pkt_idx_+1){
        flag = true;
      }
      if(!first_flag && pkthdr->is_req() && item.pkt_idx_ == 0){
        first_flag = true;
      }
    }
    sgl[0].addr = uint64_t(pkthdr);
    sgl[0].length = sizeof(pkthdr_t);
    sgl[0].lkey = pkthdr->lkey;
    ERPC_INFO("\tadded header to sgl[0]\n");

    if(pkthdr->this_msg_size != 0){
        // add ? 
        uint32_t iovec_len;
        iovec* iov = msg_buffer->get_n_data(item.pkt_idx_, /*output*/ iovec_len);
        ERPC_INFO("\tmsg_buffer->get_n_data(item.pkt_idx_ = %d, iov.iov_base = 0x%X,iovec_len = %u);\n",item.pkt_idx_,iov[0].iov_base,iovec_len);
        for(int j=0;j<iovec_len; ++j){
          sgl[j+1].addr = uint64_t(iov[j].iov_base);
          sgl[j+1].length = iov[j].iov_len;
          sgl[j+1].lkey = sgl[0].lkey; //如果头和Buffer申请到了不同片的内存，这个lkey是不同的，这里可能会有问题。
        }
        ERPC_INFO("\tadded data to sgl[1:%d]\n",iovec_len);
        wr.num_sge = iovec_len+1;
    }else{
      wr.num_sge = 1;
      wr.send_flags |= IBV_SEND_INLINE;
    }
    
    // ERPC_INFO("wr.num_sge = iovec_len; is %d\n",wr.num_sge);
    // int sum_size = 0;
    // for(int i=0;i<wr.num_sge;++i)
    //   sum_size+=(wr.sg_list+i)->length;
    // ERPC_INFO("sum_size is %d\n", sum_size);

    const auto* ib_rinfo =
        reinterpret_cast<ib_routing_info_t*>(item.routing_info_);
    wr.wr.ud.ah = ib_rinfo->ah;
    wr.wr.ud.remote_qpn = ib_rinfo->qpn;
    // if (kTesting && item.drop_) wr.wr.ud.remote_qpn = 0;
  }

  send_wr[num_pkts - 1].next = nullptr;  // Breaker of chains, first of her name

  struct ibv_send_wr* bad_wr;
  ERPC_INFO("Before send\n");
  if(record_time_flag && first_flag){
    // first_send[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
    first_send.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  }
  if(record_time_flag && flag){
    // before_send[lat_client_count-1]=double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count());
    before_send.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - lat_client[lat_client_count-1])).count()));
  }
  int ret = ibv_post_send(qp, &send_wr[0], &bad_wr);
  if (unlikely(ret != 0)) {
    fprintf(stderr, "eRPC: Fatal error. ibv_post_send failed. ret = %d\n", ret);
    assert(ret == 0);
    exit(-1);
  }
  ERPC_INFO("Send Successful\n");

  send_wr[num_pkts - 1].next = &send_wr[num_pkts];  // Restore chain; safe
}

void IBTransport::tx_flush() {
  if (unlikely(nb_tx == 0)) return;

  // If we are here, we have sent a packet. The selective signaling logic
  // guarantees that there is *exactly one* *signaled* SEND work request.
  poll_cq_one_helper(send_cq);  // Poll the one existing signaled WQE

  // Use send_wr[0] to post the second signaled flush WQE
  struct ibv_send_wr& wr = send_wr[0];
  struct ibv_sge* sgl = send_sgl[0];

  assert(wr.next == &send_wr[1]);  // +1 is valid
  assert(wr.wr.ud.remote_qkey == kQKey);
  assert(wr.opcode == IBV_WR_SEND_WITH_IMM);
  assert(wr.sg_list == send_sgl[0]);

  // We could use a header-only SEND, but the optimized inline-copy function in
  // the modded driver expects WQEs with exactly one SGE.
  char flush_inline_buf[1];
  sgl[0].addr = reinterpret_cast<uint64_t>(flush_inline_buf);
  sgl[0].length = 1;

  wr.next = nullptr;  // Break the chain
  wr.send_flags = IBV_SEND_SIGNALED | IBV_SEND_INLINE;
  wr.num_sge = 1;
  wr.wr.ud.remote_qpn = 0;  // Invalid QPN, which will cause the drop
  wr.wr.ud.ah = self_ah;    // Send to self

  struct ibv_send_wr* bad_wr;
  int ret = ibv_post_send(qp, &send_wr[0], &bad_wr);
  assert(ret == 0);
  if (unlikely(ret != 0)) {
    fprintf(stderr, "eRPC Error. tx_flush post_send() failed. ret = %d\n", ret);
    exit(-1);
  }

  wr.next = &send_wr[1];  // Restore the chain

  poll_cq_one_helper(send_cq);  // Poll the signaled WQE posted above
  nb_tx = 0;                    // Reset signaling logic

  testing_.tx_flush_count_++;
}

size_t IBTransport::rx_burst() {
  int ret = ibv_poll_cq(recv_cq, kPostlist, recv_wc);
  assert(ret >= 0);
  return static_cast<size_t>(ret);
}

void IBTransport::post_recvs(size_t num_recvs) {
  assert(!fast_recv_used);        // Not supported yet
  assert(num_recvs <= kRQDepth);  // num_recvs can be 0
  assert(recvs_to_post < kRecvSlack);

  recvs_to_post += num_recvs;
  if (recvs_to_post < kRecvSlack) return;

  if (use_fast_recv) {
    // Construct a special RECV wr that the modded driver understands. Encode
    // the number of required RECVs in its num_sge field.
    struct ibv_recv_wr special_wr;
    special_wr.wr_id = kMagicWrIDForFastRecv;
    special_wr.num_sge = recvs_to_post;

    struct ibv_recv_wr* bad_wr = &special_wr;
    int ret = ibv_post_recv(qp, nullptr, &bad_wr);
    if (unlikely(ret != 0)) {
      fprintf(stderr, "eRPC IBTransport: Post RECV (fast) error %d\n", ret);
      exit(-1);
    }

    // Reset slack counter
    recvs_to_post = 0;
    return;
  }

  // The recvs posted are @first_wr through @last_wr, inclusive
  struct ibv_recv_wr *first_wr, *last_wr, *temp_wr, *bad_wr;

  int ret;
  size_t first_wr_i = recv_head;
  size_t last_wr_i = first_wr_i + (recvs_to_post - 1);
  if (last_wr_i >= kRQDepth) last_wr_i -= kRQDepth;

  first_wr = &recv_wr[first_wr_i];
  last_wr = &recv_wr[last_wr_i];
  temp_wr = last_wr->next;

  last_wr->next = nullptr;  // Breaker of chains, queen of the First Men

  ret = ibv_post_recv(qp, first_wr, &bad_wr);
  if (unlikely(ret != 0)) {
    fprintf(stderr, "eRPC IBTransport: Post RECV (normal) error %d\n", ret);
    exit(-1);
  }

  last_wr->next = temp_wr;  // Restore circularity

  // Update RECV head: go to the last wr posted and take 1 more step
  recv_head = last_wr_i;
  recv_head = (recv_head + 1) % kRQDepth;

  // Reset slack counter
  recvs_to_post = 0;
}

}  // namespace erpc

#endif
