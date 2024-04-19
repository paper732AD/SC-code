#include "Node.h"
#include "flatLogFlushChunkRequestPB.h"
#include <string>

std::string str;
std::string str2;

iovec* str_array;


Greeter::SayHelloAll::Request get_req (int len=952)
{
    str=std::string(len,'h');
    Greeter::SayHelloAll::Request req;
    req.a.assign(str);
    return req;
}
Greeter::SayHelloAll::Response* get_resp (){
    Greeter::SayHelloAll::Response* resp;
    return resp;
}

Greeter_array::SayHelloAll_array::Request_array get_req_array (int len=952, int item=952)
{
    if(len==1024){
         RPC::string _str,_str2;
        str_array = new iovec[len/item+5];
        str = std::string(item, 'h');
        str2 = std::string(item,'h');
        _str.assign(str);
        _str2.assign(str2);
        for(int i=0, j=0;i<len;i+=item,j++){
            if(j==0)str_array[j].iov_base = (void*)(str.c_str());
            else str_array[j].iov_base = (void*)(str2.c_str());
            str_array[j].iov_len = item;
        }

        Greeter_array::SayHelloAll_array::Request_array req;
        req.a.assign(str_array, len/item);
        return req;
    }
    RPC::string _str;
    str_array = new iovec[len/item+5];
    str = std::string(item, 'h');
    _str.assign(str);
    for(int i=0, j=0;i<len;i+=item,j++){
        str_array[j].iov_base = (void*)(str.c_str());
        str_array[j].iov_len = item;
    }

    Greeter_array::SayHelloAll_array::Request_array req;
    req.a.assign(str_array, len/item);
    return req;
}
Greeter_array::SayHelloAll_array::Response_array* get_resp_array (){
    Greeter_array::SayHelloAll_array::Response_array* resp;
    return resp;
}

/********************FLAT*****************************/
static request req;
static ReplicaLocationPB replicaLocationPB;
request get_flat_req(int len){
    str = std::string(len,'h');
    std::cout << "str address:" << &str <<std::endl;
    req.Data.assign(str);
    req.HostName.assign("https://192.168.0.201:10889");
    req.UserId.assign("4f062dd5-05e5-48e3-821e-35e46ca028ce");
    req.Checksum.value = 123123123;
    req.OpenChunkOptions.ChunkOptions.Placement.assign(str);
    replicaLocationPB.CSLocation.assign(str);
    req.OpenChunkOptions.ChunkLocation.assign(&replicaLocationPB, 1);
    req.Strategy.Strategies._len = 0;
    RPC::SerializerIOV ser;
    // ser.serialize(req);

    return req;
}
static response resp;
response get_flat_resp (int len){
    str = std::string(len,'h');

    resp.trace_data.assign(str);
    return resp;
}




