#include "./header/parser.h"



int main (int argc, char*argv[]){

    GS::G_Parser parser;

    if(argc != 2){
        cout<<"Usage: ./proto_parser first_IID"<<endl;
        cout<<"\tservice1's IID = first_IID"<<endl;
        cout<<"\tservice2's IID = first_IID+1"<<endl;
    }

    parser.parser_file("/home/gs/eRPC_reading/src/huibuffer/test-gs/test_ser/proto-parser/test.proto",atoi(argv[1]));
    parser.generate_file();
    return 0;
}