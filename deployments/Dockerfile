ARG REGISTRY_HOST=docker.io
ARG IMAGE_USERNAME=ivvitc
ARG IMAGE_NAME=nos3-64
ARG IMAGE_TAG=20250217

# ARG REGISTRY_HOST=docker.io
# ARG IMAGE_USERNAME=library
# ARG IMAGE_NAME=ubuntu
# ARG IMAGE_TAG=25.04

ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

ARG GIT_URL=https://github.com/nasa/nos3
ARG GIT_BRANCH=main

ARG BUILD_DIR=/builds
ARG BASE_DIR=${BUILD_DIR}/nos3

#------------------------------------------------------------------------------
FROM ${IMAGE_URI}
#------------------------------------------------------------------------------

#--- PROXY CONFIG (if needed)
ARG MAVEN_HTTPS_PROXY
ARG HTTPS_PROXY
ARG HTTP_PROXY
ARG DEPLOYMENT_ENVIRO

ENV MAVEN_HTTPS_PROXY=$MAVEN_HTTPS_PROXY
ENV HTTPS_PROXY=$HTTPS_PROXY
ENV HTTP_PROXY=$HTTP_PROXY
ENV DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}
#--- 

#--- GIT CONFIGs
ARG GIT_URL
ARG GIT_BRANCH

ENV GIT_URL=${GIT_URL}
ENV GIT_BRANCH=${GIT_BRANCH}
#---

#--- NOS3 CONFIGs
ARG BUILD_DIR
ARG BASE_DIR

ENV BUILD_DIR=${BUILD_DIR}
ENV BASE_DIR=${BASE_DIR}
#---

RUN apt-get update && \
    apt-get -y install \
        sudo git curl vim make cmake tmux tree python3 pip iputils-ping dnsutils libgcrypt20-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p ${BUILD_DIR}

WORKDIR ${BUILD_DIR}

RUN git clone --recurse-submodules -b ${GIT_BRANCH} -j2 ${GIT_URL}

WORKDIR ${BASE_DIR}

RUN make -j2 clean

RUN make -j2 config
RUN make -j2 build-cryptolib
RUN make -j2 build-fsw
RUN make -j2 build-sim

RUN make -j2 build-test

COPY entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]
