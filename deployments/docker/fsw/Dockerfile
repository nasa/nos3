ARG REGISTRY_HOST=docker.io
ARG IMAGE_USERNAME=ivvitc
ARG IMAGE_NAME=nos3-64
ARG IMAGE_TAG=20250217

# ARG REGISTRY_HOST=docker.io
# ARG IMAGE_USERNAME=library
# ARG IMAGE_NAME=ubuntu
# ARG IMAGE_TAG=25.04

#ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

ARG IMAGE_URI=ghcr.io/haisamido/nos3-base:dev

ARG BASE_DIR=/home/nos3/builds/nos3
ARG FLIGHT_SOFTWARE=cfs 

#------------------------------------------------------------------------------
FROM ${IMAGE_URI}
#------------------------------------------------------------------------------
ARG DEBIAN_FRONTEND=noninteractive

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
ARG BASE_DIR
ARG FLIGHT_SOFTWARE 

ENV BASE_DIR=${BASE_DIR}
ENV FLIGHT_SOFTWARE=${FLIGHT_SOFTWARE}

#---

# COPY ./libuart.c  ${BASE_DIR}/fsw/apps/hwlib/fsw/linux/libuart.c

RUN mv ${BASE_DIR}/fsw/apps/hwlib/fsw/linux/libuart.c ${BASE_DIR}/fsw/apps/hwlib/fsw/linux/libuart.c.orig

WORKDIR ${BASE_DIR}

RUN make -j8 clean
RUN make -j8 prep
RUN make -j8 config
RUN make -j8 build-cryptolib
RUN make -j8 -e FLIGHT_SOFTWARE=${FLIGHT_SOFTWARE} build-fsw
RUN make -j8 build-sim
RUN make -j8 build-test

COPY entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]
