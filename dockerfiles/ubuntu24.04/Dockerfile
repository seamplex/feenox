FROM ubuntu:24.04

RUN apt update && \
    apt-get install -y \
        gcc \
        g++ \
        make \
        git \
        automake \
        autoconf \
        build-essential \
        libgsl-dev \
        libsundials-dev \
        petsc-dev \
        slepc-dev && \
    mkdir /work

RUN git clone https://github.com/seamplex/feenox.git && \
    cd feenox && \
    ./autogen.sh && \
    ./configure && \
    make

WORKDIR /work

ENTRYPOINT ["/feenox/feenox"]
