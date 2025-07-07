ARG REGISTRY_HOST=ghcr.io
ARG IMAGE_USERNAME=haisamido
ARG IMAGE_NAME=nos3-base
ARG IMAGE_TAG=dev

ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

ARG NOS3_USER=nos3
ARG FLIGHT_SOFTWARE=cfs 
ARG GSW_SOFTWARE=yamcs

################################################################################
FROM ${IMAGE_URI} AS nos3-yamcs
################################################################################
USER root

ARG DEBIAN_FRONTEND=noninteractive

ARG DEPLOYMENT_ENVIRO
ARG HTTPS_PROXY
ARG HTTP_PROXY
ARG NO_PROXY

ARG MAVEN_SETTINGS_FILE
ARG MAVEN_HTTPS_PROXY

ENV DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}
ENV HTTPS_PROXY=${HTTPS_PROXY}
ENV HTTP_PROXY=${HTTP_PROXY}
ENV NO_PROXY=${NO_PROXY}

ENV MAVEN_SETTINGS_FILE=${MAVEN_SETTINGS_FILE}
ENV MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY}

#------------------------------------------------------------------------------
# Install the GSW Software
#------------------------------------------------------------------------------
WORKDIR /home/${NOS3_USER}/builds/nos3

COPY settings.xml ${MAVEN_SETTINGS_FILE}

RUN ./scripts/gsw/gsw_${GSW_SOFTWARE}_build.sh

RUN cd /home/${NOS3_USER}/.nos3/${GSW_SOFTWARE} && \
  mvn ${MAVEN_HTTPS_PROXY} package yamcs:bundle
#------------------------------------------------------------------------------
