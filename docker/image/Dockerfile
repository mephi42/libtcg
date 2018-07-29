FROM ubuntu:18.04
ENV DEBIAN_FRONTEND=noninteractive
RUN sed -i -e 's/^# deb-src /deb-src /' /etc/apt/sources.list
RUN apt-get -y update && \
    apt-get -y upgrade && \
    apt-get -y build-dep \
        clang \
        gdb \
        llvm \
        qemu && \
    apt-get -y install \
        cmake \
        gcc-s390x-linux-gnu \
        git \
        valgrind
