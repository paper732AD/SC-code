


#include <iostream>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/descriptor.pb.h>
#include <fcntl.h>
#include <unistd.h>
#include <set>
#include <fstream>
#include "../ErrorCollect_impl.h"
#include <map>
#include <string>

using namespace google::protobuf::compiler;
using namespace google::protobuf::io;
using namespace google::protobuf;
using namespace std;



namespace GS {
    typedef const Descriptor* G_Descriptor;
    typedef const EnumDescriptor* G_EnumDescriptor;
    typedef const ServiceDescriptor* G_ServiceDescriptor;
    typedef const MethodDescriptor* G_MethodDescriptor;
    typedef const FieldDescriptor* G_FieldDescriptor;


    //parser .proto file based on service.
    class G_Parser{
        std::string ans;
        const FileDescriptor* file;
    
        std::map<std::string, std::string> mp;
        std::set<std::string> parsered_msg;

        string parser_message (G_Descriptor );
        string parser_enum (G_EnumDescriptor );
        string parser_service (G_ServiceDescriptor );
        string parser_method (G_MethodDescriptor);
        string parser_based_on_service();

        string add_message_inside(G_Descriptor);
        string add_enum_inside(G_EnumDescriptor);
    public:
        int first_IID;
        G_Parser (){
            init_mp();
            ans = "";
            first_IID=0;
        }
        G_Parser (std::string file_path,int first_IID){
            init_mp();
            ans = "";
            this->first_IID = first_IID;
            parser_file(file_path,first_IID);
        }

        virtual ~G_Parser (){}

        void init_mp (){
            mp["sint32"]="int32_t";
            mp["sint64"]="int64_t";
            mp["fixed32"]="uint32_t";
            mp["fixed64"]="uint64_t";
            mp["sfixed32"]="int32_t";
            mp["sfixed64"]="int64_t";
            mp["bytes"]="RPC::string";
            mp["string"]="RPC::string";
            mp["double"]="double";
            mp["float"]="float";
            mp["int32"]="int32_t";
            mp["int64"]="int64_t";
            mp["uint32"]="uint32_t";
            mp["uint64"]="uint64_t";
            mp["bool"]="bool";
        }

        bool append (std::string temp){
            ans += temp;
            return true;
        }


        bool parser_file  (string, int );//input file_path

        int generate_file ();
    };



}