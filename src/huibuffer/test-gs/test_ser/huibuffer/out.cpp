#include "Node.h"
#include "flatLogFlushChunkRequestPB.h"

using namespace RPC;
// single string
void ser_func (SerializerIOV& ser, Greeter::SayHelloAll::Request& req){
    ser.serialize(req);
}
// single string_array
void ser_func (SerializerIOV& ser, Greeter_array::SayHelloAll_array::Request_array& req){
    ser.serialize(req);
}
// single flat_request
void ser_func (SerializerIOV& ser, request& req){
    ser.serialize(req);
}
// single flat_response
void ser_func (SerializerIOV& ser, response& resp){
    ser.serialize(resp);
}
// single string
void block_invoke_ser (SerializerIOV& ser, Greeter::SayHelloAll::Request& req){
    ser.noop();
}
// single string_array
void block_invoke_ser (SerializerIOV& ser, Greeter_array::SayHelloAll_array::Request_array& req){
    ser.noop();
}
// single flat_request
void block_invoke_ser (SerializerIOV& ser, request& req){
    ser.noop();
}
// single flat_response
void block_invoke_ser (SerializerIOV& ser, response& resp){
    ser.noop();
}



// single string
void dser_func (SerializerIOV& ser, DeserializerIOV& dser, Greeter::SayHelloAll::Response*& resp){
    resp = dser.deserialize<Greeter::SayHelloAll::Response>(&ser.iov);
}
// single flat_response
void dser_func (SerializerIOV& ser, DeserializerIOV& dser, response*& resp){
    resp = dser.deserialize<response>(&ser.iov);
}
// single flat_request
void dser_func (SerializerIOV& ser, DeserializerIOV& dser, request*& req){
    req = dser.deserialize<request>(&ser.iov);
}
// single string_array
void dser_func (SerializerIOV& ser, DeserializerIOV& dser, Greeter_array::SayHelloAll_array::Response_array*& resp){
    resp = dser.deserialize<Greeter_array::SayHelloAll_array::Response_array>(&ser.iov);
}

// single string
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, Greeter::SayHelloAll::Response*& resp){
    dser.noop();
}
// single string_array
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, Greeter_array::SayHelloAll_array::Response_array*& resp){
    dser.noop();
}
// single flat_response
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, response*& resp){
    dser.noop();
}
// single flat_request
void block_invoke_dser (SerializerIOV& ser, DeserializerIOV& dser, request*& req){
    dser.noop();
}