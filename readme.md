# c++11 开发的一款日志发送工具
- gflags
- openssl 
- workflow 
- yaml-cpp 


下面记录的是一些工具包的搭建和测试过程实际上不需要这么多。

## 基础环境准备

```bash
docker pull actanble/centos8-nvim-zsh
```

## 第三方包下载
> 见其他依赖的文件夹
- [sqlite3 加密](./src/readme.md)


## 基础环境按安装
```bash

yum -y install openssl-devel gflags-devel gtest-devel

git clone --depth 1 https://github.com/sogou/workflow  # 接着cmake三部曲
cd workflow && mkdir build && cd build && cmake ../ && make -j$(nproc) && make install 
```

## 安装相关的依赖 
```bash

yum -y install gflags-devel glog-devel openssl-devel 

git clone --depth https://github.com/sogou/workflow
cd workflow 
mkdir build 
cd build 
cmake ../ 
make -j$(nproc)
make install

```

## gdb测试 (Centos 8)

```bash
yum debuginfo-install -y \
    libgcc-8.5.0-4.el8_5.x86_64 \
    libstdc++-8.5.0-4.el8_5.x86_64 \
    openssl-libs-1.1.1k-5.el8_5.x86_64 \
    zlib-1.2.11-17.el8.x86_64 -y 

## 这个是 tlinux4 
 dnf debuginfo-install gflags-2.2.2-4.oc9.x86_64 glibc-2.38-11.tl4.x86_64 glog-0.6.0-1.oc9.x86_64 libgcc-12.3.1.2-3.tl4.x86_64 libstdc++-12.3.1.2-3.tl4.x86_64 openssl-libs-3.0.12-14.tl4.x86_64 zlib-1.2.13-4.tl4.x86_64 -y 
```

## build sqlcipher 
```bash
git clone -b core-8-6-14 https://github.com/tcltk/tcl/ 
cd tcl/unix
./configure 
make -j10 
make install 
ln -sf /usr/local/bin/tclsh8.6 /usr/local/bin/tclsh

# git clone --depth 1 https://github.com/sqlcipher/sqlcipher
wget -c -N https://github.com/sqlcipher/sqlcipher/archive/refs/tags/v4.5.6.tar.gz
tar xf v4.5.6.tar.gz
cd sqlcipher-4.5.6
./configure --enable-tempstore=yes CFLAGS="-DSQLITE_HAS_CODEC" \
	LDFLAGS="-lcrypto"
make -j$(nproc)
make install 
```


## fmt 

```bash
wget -c -N https://github.com/fmtlib/fmt/archive/refs/tags/7.1.3.tar.gz
tar xf 7.1.3.tar.gz
cd fmt-7.1.3/ && mkdir build && cd ./build 
cmake -DCMAKE_CXX_STANDARD=11 ..
cmake --build . --config Release
cmake --install .
#cmake --install . --prefix /usr/local
```

## instal lmdb
```bash

wget -c -N https://github.com/LMDB/lmdb/archive/refs/tags/LMDB_0.9.31.tar.gz
tar xf LMDB_0.9.31.tar.gz
lmdb-LMDB_0.9.31/libraries/liblmdb/
make && make install

```


## Update 2024-07-11 
> proxy-server 

### futures 
- mysql wf_mysql
- http wf_http
- clickhouse https://github.com/ClickHouse/clickhouse-cpp
- jwt https://github.com/Thalhammer/jwt-cpp

- 需要加密两边的传输; server, aes 加密ik, sk
- 传递接口到后端的时候就进行，就进行解密，计算和返回对应的内容； 



### ubutnu install mak
```bash
sudo apt-get -y install clangd gcc g++ libglags-dev libgtest-dev 
sudo apt-get -y install cmake make libssl-dev libz-dev 
sudo apt-get -y install libsqlite-dev libworkflow-dev 
```

### tencentOS4 
```bash
yum -y install gflags-devel sqlite-devel glog-devel gtest-devel fmt-devel openssl-devel zlib-devel nss-devel pcre-devel 
yum -y install python3-devel python3-pip python3


wget -c -N https://github.com/sogou/workflow/archive/refs/tags/v0.11.6.tar.gz
```


## build clickhouse-cpp C++17

```bash
git clone https://github.com/ClickHouse/clickhouse-cpp
cd clickhouse-cpp/
mkdir build
cd build/
cmake ../
make -j10
make install
```

## build sqlitecpp
```bash

git clone --recursive https://github.com/SRombauts/SQLiteCpp 
cd SQLiteCpp/
mkdir build  && cd build/
cmake ../ 
make -j10 
make install 

```


## yaml cpp 
```bash 
git clone https://github.com/jbeder/yaml-cpp.git --depth 1 

# 注意这个的链接包是这个；
-lyaml-cpp 
https://github.com/jbeder/yaml-cpp/archive/refs/tags/0.8.0.tar.gz
https://github.com/clangd/clangd/releases/download/18.1.3/clangd-linux-18.1.3.zip

``` 

## lua devel 
- 这里推荐使用 lua-devel 这样可以外挂脚本进行使用，而不是每次都需要重新编译我们的工具
    - [测试示例](./src/tests/lua_func_test.cc)
