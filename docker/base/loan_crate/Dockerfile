FROM loan_dev

RUN rpm --import https://cdn.crate.io/downloads/yum/RPM-GPG-KEY-crate
RUN rpm -Uvh https://cdn.crate.io/downloads/yum/7/noarch/crate-release-7.0-1.noarch.rpm
RUN yum install -y crate 

# 컨테이너 start 시 실행 
RUN systemctl enable crate.service

# crash 다운로드 
RUN curl -o crash https://cdn.crate.io/downloads/releases/crash_standalone_0.23.0
RUN chmod 755 crash
RUN mv crash /usr/bin/.

