ARG REGISTRY_HOST=ghcr.io
ARG IMAGE_USERNAME=haisamido
ARG IMAGE_NAME=nos3-64
ARG IMAGE_TAG=dev
ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
ARG GIT_URL=https://github.com/nasa/nos3
ARG GIT_COMMIT=dev

ARG NOS3_USER=nos3
ARG FSW_SOFTWARE=cfs 
ARG GSW_SOFTWARE=yamcs

#------------------------------------------------------------------------------
FROM ${IMAGE_URI} AS nos3-base

ARG DEBIAN_FRONTEND=noninteractive

#------------------------------------------------------------------------------
# Proxy configs
#------------------------------------------------------------------------------
ARG MAVEN_HTTPS_PROXY
ARG HTTPS_PROXY
ARG HTTP_PROXY
ARG NO_PROXY
ARG DEPLOYMENT_ENVIRO

ENV MAVEN_HTTPS_PROXY=${MAVEN_HTTPS_PROXY}
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
ARG NOS3_USER
ARG FSW_SOFTWARE
ARG GSW_SOFTWARE
ARG GROUND_SOFTWARE
ARG LD_LIBRARY_PATH

ENV NOS3_USER=${NOS3_USER}
ENV FSW_SOFTWARE=${FSW_SOFTWARE}
ENV GSW_SOFTWARE=${GSW_SOFTWARE}
ENV GROUND_SOFTWARE=${GSW_SOFTWARE}
ENV LD_LIBRARY_PATH=${LD_LIBRARY_PATH}

#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Tools
#------------------------------------------------------------------------------
RUN apt-get update && \
  apt-get install -y sudo git curl vim make cmake tmux tree python3 pip && \
  apt-get install -y iputils-ping dnsutils lsof net-tools tshark jq && \
  apt-get install -y libgcrypt20-dev && \
  apt-get clean && \
  rm -rf /var/lib/apt/lists/*

RUN git config --global http.sslVerify false

#------------------------------------------------------------------------------
# Create a new user named ${NOS3_USER} with a home directory
#------------------------------------------------------------------------------
RUN useradd -m ${NOS3_USER}
RUN adduser ${NOS3_USER} sudo
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Switch to the newly created user
USER ${NOS3_USER}

# Create builds directory
RUN mkdir -p /home/${NOS3_USER}/builds/

WORKDIR /home/${NOS3_USER}/builds/

#------------------------------------------------------------------------------
# Clone ${GIT_URL}
#------------------------------------------------------------------------------
RUN git clone --recurse-submodules -b ${GIT_COMMIT} -j4 ${GIT_URL}

WORKDIR /home/${NOS3_USER}/builds/nos3

RUN chown -R ${NOS3_USER}:${NOS3_USER} /home/${NOS3_USER}

# commented printf(CRYPTO_PROMPT) because it fills up volume while the cryptolib is running 
COPY components/cryptolib/standalone/standalone.h ./components/cryptolib/standalone/standalone.h
COPY components/cryptolib/standalone/standalone.c ./components/cryptolib/standalone/standalone.c
#COPY components/cryptolib/standalone/standalone.c ./components/cryptolib/support/standalone/standalone.c
COPY scripts/cfg/prepare.sh                       ./scripts/cfg/prepare.sh

RUN make -j6 clean
RUN make -j6 prep
RUN make -j6 config
RUN make -j6 build-cryptolib

RUN make -j6 -e FSW_SOFTWARE=${FSW_SOFTWARE} build-fsw
RUN make -j6 build-sim
#RUN make -j6 build-test

#------------------------------------------------------------------------------
# Install the relevant GSW Software
#------------------------------------------------------------------------------
RUN ./scripts/gsw/gsw_${GSW_SOFTWARE}_build.sh

COPY entrypoint.sh /entrypoint.sh

CMD ["/entrypoint.sh"]