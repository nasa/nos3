ARG REGISTRY_HOST=docker.io
ARG IMAGE_USERNAME=ivvitc
ARG IMAGE_NAME=nos3-64
ARG IMAGE_TAG=20250217

ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

ARG GIT_URL=https://github.com/nasa-itc/42.git
ARG GIT_BRANCH=nos3-main

ARG BUILD_DIR=/builds
ARG BASE_DIR=${BUILD_DIR}/nos3

#------------------------------------------------------------------------------
FROM ${IMAGE_URI}
#------------------------------------------------------------------------------
ARG DEBIAN_FRONTEND=noninteractive

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
    apt-get -y install tmux tree iputils-ping dnsutils tshark && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN mkdir -p ${BUILD_DIR}

WORKDIR ${BUILD_DIR}

RUN git clone https://github.com/nasa-itc/42.git --depth 1 -b nos3-main
RUN cd ./42 && make -j2

WORKDIR ${BASE_DIR}

COPY ./entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]
