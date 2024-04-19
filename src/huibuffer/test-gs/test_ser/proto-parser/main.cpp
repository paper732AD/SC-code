#include <iostream>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/descriptor.pb.h>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>
#include "ErrorCollect_impl.h"

using namespace google::protobuf::compiler;
using namespace google::protobuf::io;
using namespace google::protobuf;
using namespace std;



template<typename T>                
void print (T temp){                
        std::cout << temp;
}     

#define PRINTF(temp)\
    print(temp);     \
    print("\n");

int fanhui (int t);

void Parser_message (const Descriptor* msg){

}



int main (int argc,char *argv[]){

    const FileDescriptor * first;

    int fd = open("/Users/gs/Documents/proto-parser/test.proto",O_RDONLY);//打开文件
    ZeroCopyInputStream *zinput = new FileInputStream(fd);//定义文件流
    MockErrorCollector err;
    auto *input = new Tokenizer(zinput, &err);//初始化输出流
    DescriptorPool _pool;
    
    auto *parser = new Parser();//定义解析
    parser->RecordErrorsTo(&err);
    parser->SetRequireSyntaxIdentifier(false);
    
    FileDescriptorProto file;
    if (parser->Parse(input,&file)){
        printf("success\n");
    }

    file.set_name("test.proto");//parser but has not name
    // const Descriptor* descriptor = file.GetDescriptor();
    // const Reflection* reflection = file.GetReflection();
    const FileDescriptor *_file = _pool.BuildFile(file);// dependencies should be in there first.
    first = _file;
    /***
     *  先遍历所有服务，然后根据服务中的request和response的名字找到message，然后再根据message找每一个字段都是什么东西。
     *  */
   
    for (int i = 0 ; i < _file->service_count() ; i++){
        const ServiceDescriptor *_service = _file->service(i);
        int count = _service->method_count();
        cout << _service->name() << endl;
        for(int j=0 ;i <count ; j++) {
            const MethodDescriptor* method = _service->method(j);
            PRINTF(method->name());
            const Descriptor* input = method->input_type();
            if (method->client_streaming() ){
                PRINTF("client streaming");
            }
            const Descriptor* output = method->output_type();
            if (method->server_streaming() ){
                PRINTF("server streaming");
            }
            PRINTF(input->name());
  
            for (int k = 0; k < input->field_count() ; k++){
                const FieldDescriptor* temp = input->field(k);
                print(temp->name()+" ");
                PRINTF(temp->type_name());

                if(!((std::string)"message").compare(temp->type_name())){
                    const Descriptor* _temp = temp->message_type();
                    int c = _temp->field_count();
                    for (int l = 0 ; l < c ; l ++){
                        const FieldDescriptor * __temp = _temp->field(l);
                        PRINTF(temp->name()+"\t"+__temp->name());
                        if(!((std::string)"enum").compare(__temp->type_name())){
                            const EnumDescriptor*  _temp1 = __temp->enum_type();
                            int c = _temp1->value_count();
                            for (int l = 0 ; l < c ; l++){
                            const EnumValueDescriptor* __temp1 = _temp1->value(l);
                            print("EnumValue:\t");
                            print(l);
                            PRINTF("\t"+__temp1->name());
                    }
                }
                    }
                }
                if(!((std::string)"enum").compare(temp->type_name())){
                    const EnumDescriptor*  _temp = temp->enum_type();
                    int c = _temp->value_count();
                    for (int l = 0 ; l < c ; l++){
                        const EnumValueDescriptor* __temp = _temp->value(l);
                        print("EnumValue:\t");
                        print(l);
                        PRINTF("\t"+__temp->name());
                    }
                }
            }

            PRINTF(output->name());
            for (int k = 0; k < output->field_count() ; k++){
                const FieldDescriptor* temp = input->field(k);
                print(temp->name()+" ");
                PRINTF(temp->type_name());
            }
            //    cout << SimpleBtoa(method) << endl;
                            PRINTF(" ");

        }
    }

    //遍历一个server的所有的方法




    close(fd);
    return  0;
}
