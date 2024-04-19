#!/bin/bash
set -e  # 在遇到错误时终止脚本的执行

# 进入build目录

cd ./build

# 删除当前目录下的所有文件和文件夹
rm -rf ./*

# 运行 cmake 命令
cmake ..

# 运行 make 命令
make