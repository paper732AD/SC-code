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
#include "../../../serialize.h"
#include "flatLogFlushChunkRequestPB.h"
using namespace RPC;
uint8_t *buffers = new uint8_t[100000];
request* resp;
// Deserializer dyn_deserializer;
void dser_func (SerializerIOV& ser, DeserializerIOV& dser, request*& req);
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, request*& req);
int main(int argc, char **argv)
{
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(12345);

    /* bind到本地地址，端口为12345 */
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    /* listen的backlog为1024 */
    listen(listenfd, 1024);

    clilen = sizeof(cliaddr);
    SerializerIOV ser;
    DeserializerIOV dser;
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    while(1)
    {
        memset(buffers,0,sizeof buffers);
        int result = read(connfd, buffers, 10000);
        printf("address is %p\n",buffers+result-sizeof(request));
        printf("address is %p\n",buffers);
        // auto c = reinterpret_cast<request*>(buffers+result-sizeof(request));
        // std::cout << c->Checksum.value << std::endl;
        // std::cout << result << std::endl;
        // std::cout << (buffers+result-sizeof(request)) << std::endl;
        // std::cout << "before---------\n";
        ser.iov.push_back(buffers,result);
        // std::cout << "before2---------\n";

        std::cout << ser.iov.iovcnt() << " " << ser.iov.sum() << std::endl;
        dser_func(ser, dser, resp);
        // std::cout << resp->Checksum.value << std::endl;
        break;
    }
    close(connfd);
    close(listenfd);
    exit(0);
}

