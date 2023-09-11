# NOS3 Ubuntu Dockerfile
#
# docker build -t nos3 .
# docker run -it -v /home/nos3/Desktop/github-nos3:/git nos3 /bin/bash

FROM ubuntu:20.04 AS nos1

# Enable i386 architecture
RUN dpkg --add-architecture i386

# Install required packages
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update -y \
    && apt-get install -y \
        cmake \
        g++-multilib \
        gcc-multilib \
        git \
		gdb \
        python-apt \
        python3-dev \
        python3-pip \
        dwarves \
        freeglut3-dev:i386 \
        lib32z1 \
        libboost-dev:i386 \
        libboost-system-dev:i386 \
        libboost-program-options-dev:i386 \
        libboost-filesystem-dev:i386 \
        libboost-thread-dev:i386 \
        libboost-regex-dev:i386 \
        libgtest-dev \
        libicu-dev:i386 \
        libncurses5-dev \
        libreadline-dev:i386 \
        libsocketcan-dev \
        libxerces-c-dev \
        wget \
		netcat \
    && rm -rf /var/lib/apt/lists/*

# Install Xerces-C required for NOS Engine
FROM nos1 AS nos2
RUN cd /tmp \
    && wget https://launchpad.net/ubuntu/+archive/primary/+sourcefiles/xerces-c/3.2.0+debian-2/xerces-c_3.2.0+debian.orig.tar.gz \
    && tar -xf /tmp/xerces-c_3.2.0+debian.orig.tar.gz \
    && cd /tmp/xerces-c-3.2.0 \
    && /tmp/xerces-c-3.2.0/configure --prefix=/usr CFLAGS=-m32 CXXFLAGS=-m32 \
    && cd /tmp/xerces-c-3.2.0 \
    && make install

# Configure environment
FROM nos2 AS nos3
ADD ./nos3_filestore /nos3_filestore/
RUN sed 's/fs.mqueue.msg_max/fs.mqueue.msg_max=500/' /etc/sysctl.conf \
    && apt-get install -y \
        /nos3_filestore/packages/ubuntu/itc-common-Release_1.10.1_i386.deb \
        /nos3_filestore/packages/ubuntu/nos-engine-Release_1.6.1_i386.deb

RUN mkdir -p /opt/nos3 \
    && cd /opt/nos3 \
    && git clone https://github.com/ericstoneking/42.git \
    && cd /opt/nos3/42 \
    && git checkout f20d8d517b352b868d2f45ee3bffdb7deeedb218

RUN cd /opt/nos3/42 \
    && sed 's/ARCHFLAG =/ARCHFLAG=-m32 /; s/LFLAGS = -L/LFLAGS = -m32 -L/; s/#GLUT_OR_GLFW = _USE_GLUT_/GLUT_OR_GLFW = _USE_GLUT_/' -i /opt/nos3/42/Makefile \
    && make \
    && ln -s /usr/lib/libnos_engine_client.so /usr/lib/libnos_engine_client_cxx11.so

