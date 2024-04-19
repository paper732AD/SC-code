#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h> /* for bzero() */
#include <string.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/uio.h> /* for iovec{} and readv/writev */
#include <chrono>
#include <gflags/gflags.h>
#include "../src/huibuffer/flat_log.h"
#include "../src/huibuffer/serialization.h"
#include "../src/util/timer.h"
using namespace erpc;
request* req;
response* resp;
erpc::Serializer reqser;
erpc::Serializer respser;
std::ofstream outfile;
std::ofstream outfile_detail;
erpc::Deserializer deserializer;
std::string str;
ReplicaLocationPB *replicaLocationPB;
uint8_t *buffers = new uint8_t[1148100];
tmpoint st;
int c;
DEFINE_uint32(data_len, 1024, "the string length in ctrl_bufer, 0-102410\n");
DEFINE_string(log_file, "latency_test.txt", "the string length in ctrl_bufer, 0-102410\n");
DEFINE_string(detail_file, "latency_test_detail.txt", "the string length in ctrl_bufer, 0-102410\n");
DEFINE_uint32(total_requests, 100, "the number of request\n");
DEFINE_uint32(port, 31860, "the port of socket listen\n");
#define INTERFAXENAME "ens6np0"
request* get_req(int len)
{
    reqser.clear();
    str = std::string(len,'h');
    req->Data.assign(str);
    req->HostName.assign("https://192.168.0.201:10889");
    req->UserId.assign("4f062dd5-05e5-48e3-821e-35e46ca028ce");
    req->Checksum.value = 123123123;
    req->OpenChunkOptions.ChunkOptions.Placement.assign(str);
    replicaLocationPB->CSLocation.assign(str);
    req->OpenChunkOptions.ChunkLocation.assign(replicaLocationPB, 1);
    req->Strategy.Strategies._len = 0;
}
int total_size;
int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    // outfile.open(FLAGS_log_file+std::to_string(FLAGS_data_len));
    // outfile_detail.open(FLAGS_detail_file+std::to_string(FLAGS_data_len));
    outfile.open(FLAGS_log_file);
    outfile_detail.open(FLAGS_detail_file);
    req = new request();
    replicaLocationPB = new ReplicaLocationPB();
    req = get_req((FLAGS_data_len-495)/3);
    reqser.serialize(*req);
    // 建立连接
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        // printf("create socket fail\n");
    }
        int sndbuf;
    socklen_t optlen = sizeof(sndbuf);

    // 获取发送缓冲区大小
    if (getsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen) == 0) {
        // printf("发送缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取发送缓冲区大小失败");
    }
    if (getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &optlen) == 0) {
        // printf("接收缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取接收缓冲区大小失败");
    }
    int new_sndbuf = 1024*1024*1024; 
    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &new_sndbuf, sizeof(new_sndbuf)) == 0) {
        // printf("发送缓冲区大小已设置为 %d\n", new_sndbuf);
    } else {
        perror("设置发送缓冲区大小失败");
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &new_sndbuf, sizeof(new_sndbuf)) == 0) {
        // printf("接收缓冲区大小已设置为 %d\n", new_sndbuf);
    } else {
        perror("设置发送缓冲区大小失败");
    }
    if (getsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen) == 0) {
        // printf("发送缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取发送缓冲区大小失败");
    }
    if (getsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &optlen) == 0) {
        // printf("接收缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取接收缓冲区大小失败");
    }
    struct ifreq interface;
    strncpy(interface.ifr_ifrn.ifrn_name, INTERFAXENAME, sizeof(INTERFAXENAME));
    if (setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface))  < 0) {
           perror("SO_BINDTODEVICE failed");
           exit(0);
    }
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FLAGS_port);
    server_addr.sin_addr.s_addr = inet_addr("192.168.0.204");
    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0)
    {
        std::cout << "failed\n";
        perror("connect failed ");
    }
    latency.reserve(FLAGS_total_requests + 1);
    // 尝试成功数
    while(1){
        total_size = FLAGS_data_len;
        memset(buffers,0,sizeof buffers);
        reqser.clear();
        // 开始计时
        st = NOW();
        // 序列化
        reqser.serialize(*req);
        // std::cout << reqser.iov.iovcnt() << std::endl;
        if (c = writev(socket_fd, reqser.iov.iovec(), reqser.iov.iovcnt()),c<0)
        {
            std::cout << "error\n";
            perror("writev failure");
            // exit(0);
        }
        if(c>0){
            // std::cout << reqser.iov.sum() << std::endl;
            // std::cout << c << std::endl;
            // std::cout << "发送消息成功\n";
            // std::cout << "finish send\n";
            // c = 0;
            // while(c==0){
            //     c = read(socket_fd, buffers, 1048000);
            // }
            int recv_size = 0;
            int result = 0;

            while( recv_size < FLAGS_data_len-20){
                // std::cout << "read中..." << std::endl;
                // result = recv(connfd, buffers+recv_size, 1124000,MSG_WAITALL);
                result = recv(socket_fd, buffers+recv_size, total_size,0);
                recv_size+=result;
                // total_size-=result;
                // std::cout << recv_size << std::endl;
            }
            // std::cout << "finish" << latency.size() << std::endl;
            // c = read(socket_fd, buffers, 1048000);
            // c = recv(socket_fd, buffers, 1124000,MSG_WAITALL);
            // if( c = recv(socket_fd, buffers, 1024000,MSG_WAITALL),c<0){ // 读取服务端发送来的回复
            //     std::cout << "error\n";
            //     perror("read failure");
            //     exit(0);
            // }
            // std::cout << recv_size << std::endl;
            if(recv_size>0){
                // std::cout << "收到了服务端的回复----\n";
                // std::cout << recv_size << std::endl;
                respser.iov.push_back(buffers,recv_size);

                // 反序列化
                resp =  deserializer.deserialize<response>(&respser.iov);
                // std::cout << resp->trace_data.back() << std::endl;
                latency.push_back(double((std::chrono::duration_cast<Nanoseconds>(NOW() - st)).count()));

                // auto ed = NOW();
            }
        }
        // break;
        if(latency.size()==FLAGS_total_requests) break;

    }
    output_latency(outfile, FLAGS_total_requests);
    write_detail_to_file(outfile_detail, latency);
    // 关闭连接
    close(socket_fd);
    return 0;
}
