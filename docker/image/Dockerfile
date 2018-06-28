FROM ubuntu:18.04
RUN sed -i -e 's/^# deb-src /deb-src /' /etc/apt/sources.list
RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y build-dep \
        llvm \
        qemu && \
    apt-get -y install \
        clang \
        cmake \
        gcc-s390x-linux-gnu \
        git \
        valgrind
