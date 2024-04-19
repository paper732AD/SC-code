# zRPC

zRPC: a zero-copy RPC system

# System Dependencies

Ubuntu 22.04 (other versions have not been tested)

# Hardware Requirements

We use two servers, each equipped with 40-
core 2.3 GHz CPUs and 32GB DRAM. One node is used
as the server of RPC, the other is used as the client. These
servers are connected by Mellanox ConnectX-5 100Gb
NICs and a 100Gb Mellanox SN2700 Switch.

# Software dependencies

```shell
# install and compile Protobuf
git clone https://github.com/protocolbuffers/protobuf.git --recursive
cd protobuf
gut checkout v3.21.1
bazel build :protoc :protobuf
sudo cp bazel-bin/protoc /usr/local/bin

# install and compile Flatbuffers
git clone https://github.com/google/flatbuffers.git
cd flatbuffers
git checkout v22.9.2
cmake -G "Unix Makefiles" .
make -j
sudo make install -j
sudo ldconfig

# install and compile grpc
git clone https://gitHub.com/grpc/grpc.git 
cd grpc
git checkout v1.56.0 
git submodule sync 
git submodule update --init 
mkdir build
cd build
cmake ..
sudo make -j
sudo make install

# install and compile brpc
sudo apt-get install -y git g++ make libssl-dev libgflags-dev libprotobuf-dev libprotoc-dev protobuf-compiler libleveldb-dev
sudo apt install make
sudo apt install gcc
sudo apt install g++
sudo apt install libleveldb-dev
sudo apt install libgflags-dev
sudo apt install openssl
sudo apt install libssl-dev
git clone https://github.com/apache/brpc.git
cd brpc
git checkout v1.3.0
sh config_brpc.sh --headers="/usr/include /usr/local/include" --libs="/usr/lib /usr/local/lib"
sudo make -j
```

# building

```shell
bash [cmake_warn.sh] [cmake_info.sh] [cmake_error.sh] # different log_level
make
```

# Running

First you should set your ip address which in `hello_world/common.h`.

- Server address : `kServerHostname`, Server Port : `kUDPPort_s`
- Client address : `kClientHostname`, Client Port : `kUDPPort_c`.

bRPC and gRPC in `RPC_test`.
zRPC in `test_xrpc`.
Serialization in `src/huibuffer/test-gs`.
