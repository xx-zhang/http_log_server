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

## 基础环境按安装
```bash
# centos 8 
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

### ubutnu install 
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
