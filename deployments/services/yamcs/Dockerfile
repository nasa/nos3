ARG REGISTRY_HOST=ghcr.io
ARG IMAGE_USERNAME=haisamido
ARG IMAGE_NAME=nos3-base
ARG IMAGE_TAG=dev

ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

#------------------------------------------------------------------------------
ARG GIT_URL=https://github.com/nasa-itc/yamcs-nos3
ARG GIT_COMMIT=dev

#------------------------------------------------------------------------------
ARG NOS3_USER=nos3
ARG FLIGHT_SOFTWARE=cfs 
ARG GSW_SOFTWARE=yamcs

################################################################################
FROM ${IMAGE_URI} AS nos3-yamcs
################################################################################
USER root

ARG DEBIAN_FRONTEND=noninteractive

#------------------------------------------------------------------------------
# Proxy configs
#------------------------------------------------------------------------------
ARG MAVEN_HTTPS_PROXY
ARG MAVEN_REPO_LOCAL
ARG COMPONENT_DIR

ARG HTTPS_PROXY
ARG HTTP_PROXY
ARG NO_PROXY
ARG DEPLOYMENT_ENVIRO

# =/home/${NOS3_USER}/.m2/repository
ENV MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY}
ENV MAVEN_REPO_LOCAL=${MAVEN_REPO_LOCAL}
ENV COMPONENT_DIR=${COMPONENT_DIR}

ENV HTTPS_PROXY=${HTTPS_PROXY}
ENV HTTP_PROXY=${HTTP_PROXY}
ENV NO_PROXY=${NO_PROXY}
ENV DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}

#------------------------------------------------------------------------------
# Git Configs
#------------------------------------------------------------------------------
ARG GIT_URL
ARG GIT_COMMIT

ENV GIT_URL=${GIT_URL}
ENV GIT_COMMIT=${GIT_COMMIT}

#------------------------------------------------------------------------------
# NOS3 Configs
#------------------------------------------------------------------------------
ARG FLIGHT_SOFTWARE
ARG GSW_SOFTWARE
ARG NOS3_USER

ENV NOS3_USER=${NOS3_USER}
ENV FLIGHT_SOFTWARE=${FLIGHT_SOFTWARE}
ENV GSW_SOFTWARE=${GSW_SOFTWARE}
#------------------------------------------------------------------------------

  # Switch to the newly created user
USER ${NOS3_USER}

#------------------------------------------------------------------------------
# Install the GSW Software
#------------------------------------------------------------------------------
WORKDIR /home/${NOS3_USER}/.nos3
RUN rm -rf /home/${NOS3_USER}/.nos3/${GSW_SOFTWARE}
RUN git clone --recurse-submodules -b ${GIT_COMMIT} -j4 ${GIT_URL} ${GSW_SOFTWARE} && \
  cd ./${GSW_SOFTWARE} && \
    git fetch && \
    git pull origin 

WORKDIR /home/${NOS3_USER}/.nos3/${GSW_SOFTWARE}
COPY settings.xml ./settings.xml

RUN mvn -X ${MAVEN_HTTPS_PROXY} -DskipTests -Dmaven.repo.local=${MAVEN_REPO_LOCAL} -DCOMPONENT_DIR=${COMPONENT_DIR} install

#------------------------------------------------------------------------------
