FROM loan_base

# gflags 컴파일
RUN git clone https://github.com/gflags/gflags.git
RUN mkdir -p gflags/cbuild
RUN cd gflags/cbuild && cmake .. && make && make install

# glog 컴파일
RUN git clone https://github.com/google/glog.git
RUN mkdir -p glog/cbuild
RUN cd glog/cbuild && cmake .. && make && make install

RUN echo "alias ll='ls -al'" >> ~/.bashrc
RUN source ~/.bashrc

# 하기 포트를 외부로 노출합니다.
EXPOSE 3333 
EXPOSE 4444
EXPOSE 5555
