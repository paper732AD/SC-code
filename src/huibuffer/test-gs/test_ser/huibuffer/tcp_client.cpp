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
#include "../../../serialize.h"
#include "flatLogFlushChunkRequestPB.h"
using namespace RPC;

request req;
request* resp;
struct iovec iov[2300];

int main(int argc, char **argv)
{
    // if (argc != 2)
    // {
    //     perror("usage: batchwrite <IPaddress>");
    // }
            // 得到req
    req = get_flat_req(1024);
    SerializerIOV ser;
    ser.serialize(req);
    int iov_sum = ser.iov.iovcnt();
    for(int i = 0 ; i < iov_sum ; i ++){
        iov[i] = ser.iov[i];
        std::cout << iov[i].iov_base << " " << iov[i].iov_len << std::endl;
    } 

    // 建立连接
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        printf("create socket fail\n");
    }
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    if (connect_rt < 0)
    {
        std::cout << "failed\n";
        perror("connect failed ");
    }
    int cnt = 1;
    std::cout << ser.iov.sum() << std::endl;
    for(int i = 0 ; i < cnt ; i ++){
        // 得到req
        // req = get_flat_req(1024);
        // SerializerIOV ser;
        // std::cout << ser.iov[]
        // ser.serialize(req);
        int c;
        auto last = ser.iov[ser.iov.iovcnt()-1];
        std::cout << last.iov_base << " " << last.iov_len << std::endl;
        if (c = writev(socket_fd, iov, ser.iov.iovcnt()),c<0)
        {
           std::cout << "error\n";
           perror("writev failure");
        }
        std::cout << c << std::endl;
        std::cout << "finish\n";
    }
    // 关闭连接
    close(socket_fd);
    exit(0);
}
