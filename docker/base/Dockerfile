FROM centos:7

RUN yum install -y vim net-tools make cmake autoconf git wget curl boost boost-devel openssl openssl-devel
#RUN yum install -y gcc* gdb* 
RUN yum install -y gcc*
RUN yum install -y gdb*

# cmake 최신 설치 
RUN git clone https://github.com/Kitware/CMake.git
RUN cd CMake && ./bootstrap && make && make install

# gflags 설치
RUN git clone https://github.com/gflags/gflags.git
RUN cd gflags && mkdir cbuild && cd cbuild && cmake .. && make && make install

# protobuf 설치
RUN git clone https://github.com/protocolbuffers/protobuf.git
RUN mkdir -p protobuf/cmake/cbuild
RUN cd protobuf/cmake/cbuild && cmake -Dprotobuf_BUILD_TESTS=OFF .. && make && make install

ENV PYTHONUNBUFFERED=0
