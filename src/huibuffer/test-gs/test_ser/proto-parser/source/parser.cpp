#include "../header/parser.h"
#include "../ErrorCollect_impl.h"
#include <fcntl.h>
#include <map>
#include <set>

/***
 * struct service {                 //step 1
 *      struct internal_struct {}   //step 2
 * 
 * 
 *  struct method1 {                //step 3
 *      struct request : public message {//step4
 *             internal_struct temp; //step5
 *      }
 * 
 *      struct response : public message {}
 *  }
 *  struct method2 {                
 * 
 *      struct request : public message {}
 * 
 *      struct response : public message {}
 *  }
 * }
 * */





namespace GS{

#define LOG(temp) cout<<temp
#define LOGF(temp) LOG(temp)<<endl
#define ERROR_LOG(temp) cout<<temp
#define ERRORF_LOG(temp) ERROR_LOG(temp)<<endl
    
    //format the file by tab(\t)
    std::string t(int n){
        std::string temp = "";
        for (int i = 0; i<n ;i++){
            temp+="\t";
        }
        return temp;
    }
    int FID_COUNT = 0;

    /**
     * @brief parser——file，is public
     * 
     * @param file_path proto file' path
     * @return true 
     * @return false 
     */
    bool G_Parser::parser_file (string file_path,int first_IID){
        this->first_IID = first_IID;
        //open file
        int fd = open(file_path.c_str(),O_RDONLY);
        ZeroCopyInputStream *zinput = new FileInputStream(fd);
        //class for collect error information, need to identify by user
        MockErrorCollector err;
        Tokenizer *input = new Tokenizer(zinput, &err);

        //this pool save the dependencies for file,
        //but this version is for single file,
        //so the pool is useless except transfer FileDescirptorProto into FileDescriptor
        DescriptorPool _pool;
        auto* parser = new Parser();
        //set the error collector
        parser->RecordErrorsTo(&err);
        parser->SetRequireSyntaxIdentifier(false);

        FileDescriptorProto _file;
        if(parser->Parse(input,&_file)){
            LOG("success parser\n");
        }else{
            ERROR_LOG("parser failed\n");
            return false;
        }
        //must has its own name! but why?
        _file.set_name(file_path);

        //dependences should be in the pool first.
        //tranfer _file(FileDescriptorProto) into file(FileDescriptor)
        file = _pool.BuildFile(_file);

        //parser file!
        this->append(parser_based_on_service());

        // printf("%s\n",ans.c_str());
        
        return true;
    }

    string G_Parser::parser_based_on_service (){
        std::string temp="";

        //parser based on services.
        int c = file->service_count();
    // struct P2P_Interface
    // {
    //     const static uint32_t IID = IID_P2P;
    // };
        for (int i = 0; i < c; i++){
            //process each service in file
            G_ServiceDescriptor _gservice = file->service(i);
            ans+="struct P2P_Interface\n{\n\tconst static uint32_t IID = "+
                std::to_string(first_IID++)+";\n};\n";
            //step 1 add struct service
            ans += "struct "+ _gservice->name() + ": public P2P_Interface" + " {\n";
            //fille struct by context
            ans += parser_service(_gservice);
            //add tail 
            ans += "}\n";
        }

        return temp;
    }


    string G_Parser::parser_service (G_ServiceDescriptor gservice){
        std::string temp="";
        //foreach every method in gservice.
        int c = gservice->method_count();
        for (int i = 0 ; i<c ; i++){
            //each method is saved in _gmethod,
            //and the parser the method.
            G_MethodDescriptor _gmethod = gservice->method(i);
            temp += parser_method(_gmethod);
        }
        return temp;
    }

    //暂时没考虑stream
    string G_Parser::parser_method (G_MethodDescriptor gmethod){
        //add struct method  step3
        std::string temp=t(1)+"struct "+gmethod->name()+" {\n";
            temp += t(2)+"const static uint32_t FID = "+std::to_string(FID_COUNT++)+";\n";
            //add struct request inside struct gmethod step4
            temp+=t(2)+"struct request: public RPC::message{\n";
                //fille requets & response bellow.
                temp+=parser_message(gmethod->input_type());
            temp+=t(2)+"};\n";

            temp+=t(2)+"struct response: public RPC::message{\n";
                temp+=parser_message(gmethod->output_type());
            temp+=t(2)+"};\n";

        //add tail
        temp += t(1)+"};\n";
        return temp;
    }

    //add line like "int32 number;" in request, request or message 
    string inside_variable (std::string type, std::string name){
        char temp[type.size() + name.size() + 10];
        sprintf(temp,"%s %s;",type.c_str(),name.c_str());
        return t(3)+(std::string)temp+"\n";
    }
    //add line like "array<int32> list;" in request, request or message 
    string repeated_inside_variable(std::string type, std::string name){
        char temp[type.size() + name.size() + 20];
        sprintf(temp,"RPC::array<%s> %s;",type.c_str(),name.c_str());
        return t(3)+(std::string)temp+"\n";
    }
    //revoke the two function upon
#define REP_OR_NOT(type,name)                               \
            if(gfield->is_repeated())                       \
                temp += repeated_inside_variable(type,name);\
            else                                            \
                temp += inside_variable(type,name);         \
    //parser message in file, like input and output in service to req&resp struct,
    // or message in intput&output struct to service struct.
    string G_Parser::parser_message (G_Descriptor gmsg){
        std::string temp="";
        //c is the field number in gmsg.
        int c = gmsg->field_count();

        for (int i = 0; i<c; i++){
            //each field is saved in gfield.
            G_FieldDescriptor gfield = gmsg->field(i);

            //if this field is a message;
            if(!((std::string)"message").compare(gfield->type_name())){
                G_Descriptor _gmsg = gfield->message_type();
                //if it has add in the service struct already,
                // it need not to process again.
                if (!this->parsered_msg.count(_gmsg->name()))
                    add_message_inside(_gmsg);

                //fill it into req&resp struct.
                REP_OR_NOT(mp[_gmsg->name()],gfield->name())
            }
            //if it is "enum" field
            else if(!((std::string)"enum").compare(gfield->type_name())){
                //process it by a specific function.
                if(!mp.count(gfield->enum_type()->name()))
                    add_enum_inside(gfield->enum_type());
                REP_OR_NOT(mp[gfield->enum_type()->name()],gfield->name())
            }
            //else, put it into req&resp struct directly.
            else{
                REP_OR_NOT(mp[gfield->type_name()],gfield->name())
            }
        }

        temp+=t(3)+"PROCESS_FIELDS(";
        for(int c=0; c<gmsg->field_count() ; c++){
            temp += gmsg->field(c)->name();
            if(c<gmsg->field_count()-1)
                temp += ",";
        }
        temp+=");\n";

        return temp;
    }


    //parser enum struct
    string G_Parser::parser_enum (G_EnumDescriptor genum){
        std::string temp="";
        temp += t(1)+"enum "+ genum->name()+" { ";
        int c = genum->value_count();
        for (int i= 0;i<c;i++){
            //check every unit.
            temp+=genum->value(i)->name()+(std::string)"="+(std::string)std::to_string(genum->value(i)->number());
            if(i<c-1)
                temp+=",";
        }
        temp +="};\n";
        return temp;
    }


    //step 2 step2 add single message into service strcut directly 
    string G_Parser::add_message_inside(G_Descriptor gmsg){
        mp[gmsg->name()]=gmsg->name();
        std::string temp="";
        temp+=t(1)+"struct "+gmsg->name()+" {\n";
            int c = gmsg->field_count();
            for (int i=0;i<c;i++){
                G_FieldDescriptor gfield = gmsg->field(i);

                if(!((std::string)"message").compare(gfield->type_name())){
                    G_Descriptor _gmsg = gfield->message_type();
                    if(!mp.count(_gmsg->name()))
                        add_message_inside(_gmsg);
                    REP_OR_NOT(mp[_gmsg->name()],gfield->name())
                }else if(!((std::string)"enum").compare(gfield->type_name())){
                    if(!mp.count(gfield->enum_type()->name()))
                        add_enum_inside(gfield->enum_type());
                    REP_OR_NOT(mp[gfield->enum_type()->name()],gfield->name())
                }else{
                    // if(!mp.count(gfield->type_name())){ /*** useless ***/
                    //     add_message_inside(file->FindMessageTypeByName(gfield->type_name()));
                    //     mp[gfield->type_name()]=gfield->type_name();
                    // }
                    REP_OR_NOT(mp[gfield->type_name()],gfield->name())
                }
            }

        temp+=t(1)+"};\n";
        ans += temp;
        parsered_msg.insert(gmsg->name());
        return "";
    }

    string G_Parser::add_enum_inside(G_EnumDescriptor genum){
        ans += parser_enum(genum);
        mp[genum->name()]=genum->name();
        return "";
    }


    //write into file .
    int G_Parser::generate_file (){

        LOGF("ans = \n");
        LOG(ans);
        int fd = open("/home/gs/eRPC_reading/src/huibuffer/test-gs/test_ser/proto-parser/generate.h",O_RDWR);
        write(fd,this->ans.c_str(),ans.size());
        close(fd);

        return 0;
    }
}