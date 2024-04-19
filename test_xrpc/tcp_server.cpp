#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h> /* for bzero() */
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include <string.h>
#include <gflags/gflags.h>
#include "../src/huibuffer/flat_log.h"
#include "../src/huibuffer/serialization.h"
using namespace erpc;
uint8_t *buffers = new uint8_t[1148100];
response* resp;
request* req;
erpc::Deserializer deserializer;
erpc::Serializer respser;
erpc::Serializer reqser;
std::string data;
int result;
DEFINE_uint32(data_len, 1024, "the string length in flag_log_resp, 0-10240\n");
DEFINE_uint32(port, 31860, "the port of socket listen\n");
#define INTERFAXENAME "enp152s0np0"

void init(int len)
{
    resp = new response();
    data = std::string(len-80,'h');
    resp->trace_data.assign(data);
}
// struct iovec iovc[2000];
int total_size;
int main(int argc, char **argv)
{
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    init(FLAGS_data_len);
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    int on = 1;
    // 复用
    int ret = setsockopt( listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

    if(listenfd<0){
        std::cout << "socket error\n";
    }

    int sndbuf;
    socklen_t optlen = sizeof(sndbuf);
    // 获取发送缓冲区大小
    if (getsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen) == 0) {
        printf("发送缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取发送缓冲区大小失败");
    }
    if (getsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &optlen) == 0) {
        printf("接收缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取接收缓冲区大小失败");
    }
    int new_sndbuf = 1024*1024*1024; 
    if (setsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &new_sndbuf, sizeof(new_sndbuf)) == 0) {
        printf("发送缓冲区大小已设置为 %d\n", new_sndbuf);
    } else {
        perror("设置发送缓冲区大小失败");
    }
    if (setsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &new_sndbuf, sizeof(new_sndbuf)) == 0) {
        printf("接收缓冲区大小已设置为 %d\n", new_sndbuf);
    } else {
        perror("设置发送缓冲区大小失败");
    }
    if (getsockopt(listenfd, SOL_SOCKET, SO_SNDBUF, &sndbuf, &optlen) == 0) {
        printf("发送缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取发送缓冲区大小失败");
    }
    if (getsockopt(listenfd, SOL_SOCKET, SO_RCVBUF, &sndbuf, &optlen) == 0) {
        printf("接收缓冲区大小: %d\n", sndbuf);
    } else {
        perror("获取接收缓冲区大小失败");
    }
    struct ifreq interface;
    strncpy(interface.ifr_ifrn.ifrn_name, INTERFAXENAME, sizeof(INTERFAXENAME));
    if (setsockopt(listenfd, SOL_SOCKET, SO_BINDTODEVICE, (char *)&interface, sizeof(interface))  < 0) {
           perror("SO_BINDTODEVICE failed");
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(FLAGS_port);



    /* bind到本地地址，端口为13888 */
    int bind_z = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if(bind_z<0){
        std::cout << bind_z << std::endl;
        std::cout << "bind error\n";
    }
    /* listen的backlog为1024 */
    if(listen(listenfd, 1024)<0){
        std::cout << "listen error\n";
    }

    clilen = sizeof(cliaddr);
    
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    if(connfd<0){
        std::cout << "accept error\n";
    }
    while(1)
    {
        memset(buffers,0,sizeof buffers);

        total_size = FLAGS_data_len;
        result = 0;

        reqser.clear();
        int recv_size = 0;
        while( recv_size < FLAGS_data_len-10){
            // result = recv(connfd, buffers+recv_size, 1124000,MSG_WAITALL);
            result = recv(connfd, buffers+recv_size, total_size,0);
            recv_size+=result;
            // std::cout << recv_size << std::endl;
            // total_size-=result;
        }
        // std::cout << recv_size << std::endl;
        // std::cout<< "finish recv \n";
        // result = recv(connfd, buffers, 1124000,MSG_WAITALL);
            // recv
            // std::cout << "-----进入3\n";
       
        // std::cout << recv_size << std::endl;
        if(recv_size>0){
            reqser.iov.push_back(buffers,recv_size);
            // std::cout << recv_size << "\n";
            // 反序列化
            req = deserializer.deserialize<request>(&reqser.iov);
            // std::cout << req->Data.back() << std::endl;
            // 回复消息........
            respser.clear();
            respser.serialize(*resp);
            // std::cout << respser.iov.sum() << std::endl;
            // std::cout << respser.iov.iovcnt() << std::endl;
            // for(int i = 0 ; i < respser.iov.iovcnt() ; i ++){
            //     std::cout << respser.iov[i].iov_len << std::endl;
            //     // std::cout << respser.iov.iovec()->iov_base << std::endl;
            //     // iovc[i] = respser.iov[i];
            // }
            int response_sum = writev(connfd,respser.iov.iovec(),respser.iov.iovcnt());
            if(response_sum==0){
                std::cout << "回复消息时出现了错误\n";
                exit(0);
            }
            // std::cout << "发了这么多数据"<<std::endl;
            // std::cout << response_sum << std::endl;
            // sleep(5);
        }
        // sleep(2);
        // break;
    }
    close(listenfd);
    close(connfd);
    return 0;
}

