FROM centos

RUN yum install -y vim net-tools gcc* gdb* make cmake autoconf git wget curl boost boost-devel openssl openssl-devel

# gflags 설치
RUN wget https://download-ib01.fedoraproject.org/pub/epel/7/x86_64/Packages/g/gflags-2.1.1-6.el7.x86_64.rpm
RUN wget https://download-ib01.fedoraproject.org/pub/epel/7/x86_64/Packages/g/gflags-devel-2.1.1-6.el7.x86_64.rpm
RUN rpm -Uvh gflags-2.1.1-6.el7.x86_64.rpm gflags-devel-2.1.1-6.el7.x86_64.rpm

# cmake 최신 설치 
RUN git clone https://github.com/Kitware/CMake.git
RUN cd CMake && ./bootstrap && make && make install

# protobuf 설치
RUN git clone https://github.com/protocolbuffers/protobuf.git
RUN mkdir -p protobuf/cmake/cbuild
RUN cd protobuf/cmake/cbuild && cmake -Dprotobuf_BUILD_TESTS=OFF .. && make && make install

ENV PYTHONUNBUFFERED=0
