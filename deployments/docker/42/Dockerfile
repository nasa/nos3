ARG REGISTRY_HOST=ghcr.io
ARG IMAGE_USERNAME=haisamido
ARG IMAGE_NAME=nos3-base
ARG IMAGE_TAG=dev

ARG IMAGE_URI=${REGISTRY_HOST}/${IMAGE_USERNAME}/${IMAGE_NAME}:${IMAGE_TAG}

ARG GIT_URL=https://github.com/nasa-itc/42.git
ARG GIT_BRANCH=nos3-main

ARG VNC_PASSWORD=0123456789

################################################################################
FROM ${IMAGE_URI} AS x-vnc
################################################################################
USER root

ARG DEBIAN_FRONTEND=noninteractive

ARG DEPLOYMENT_ENVIRO
ARG HTTPS_PROXY
ARG HTTP_PROXY
ARG NO_PROXY

ENV DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}
ENV HTTPS_PROXY=${HTTPS_PROXY}
ENV HTTP_PROXY=${HTTP_PROXY}
ENV NO_PROXY=${NO_PROXY}

ARG VNC_PASSWORD
ENV VNC_PASSWORD=${VNC_PASSWORD}

RUN apt-get update && \
    apt-get install -y \
      gnupg \
      wget \
      apt-transport-https \
      software-properties-common \
      novnc \
      websockify \
      libxv1 \
      libglu1-mesa \
      xauth \
      x11-utils \
      xterm \
      xorg \
      tightvncserver && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# turbovnc: https://turbovnc.org/
RUN wget -q -O- https://packagecloud.io/dcommander/turbovnc/gpgkey |  gpg --dearmor >/etc/apt/trusted.gpg.d/TurboVNC.gpg
RUN wget -P /etc/apt/sources.list.d/ https://raw.githubusercontent.com/TurboVNC/repo/main/TurboVNC.list

# virtualgl: https://virtualgl.org/
RUN wget -q -O- https://packagecloud.io/dcommander/virtualgl/gpgkey | gpg --dearmor >/etc/apt/trusted.gpg.d/VirtualGL.gpg
RUN wget -P /etc/apt/sources.list.d/ https://raw.githubusercontent.com/VirtualGL/repo/main/VirtualGL.list

# Install virtualgl and turbovnc
RUN apt-get update && \
    apt-get install -y \
      virtualgl \
      turbovnc && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install and configure Display Manager(s) - https://fluxbox.org/
RUN apt-get update && \
    apt-get install -y \
      fluxbox && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# TODO: need to figure out why this is needed
RUN ln -sf /usr/share/xsessions/fluxbox.desktop /usr/share/xsessions/gnome.desktop

# Install tools for git cloning and development, etc.
RUN apt-get update && \
    apt install -y curl git build-essential vim jq tree tmux htop bash-completion dos2unix netcat-traditional iputils-ping && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install https://taskfile.dev/ (Taskfile.yaml)
RUN sh -c "$(curl --location https://taskfile.dev/install.sh)" -- -d

# Install some useful .bashrc-isms
# COPY .bashrc.local /root/.bashrc.local
# RUN cat /root/.bashrc.local >> /root/.bashrc

# VNC Config
RUN mkdir -p ~/.vnc/
RUN echo ${VNC_PASSWORD} | vncpasswd -f > ~/.vnc/passwd
RUN chmod 0600 ~/.vnc/passwd
RUN openssl req -x509 -nodes -newkey rsa:2048 -keyout ~/novnc.pem -out ~/novnc.pem -days 3650 -subj "/C=US/ST=NY/L=NY/O=NY/OU=NY/CN=NY emailAddress=email@example.com"

################################################################################
FROM x-vnc AS fortytwo
################################################################################
ARG GIT_URL
ARG GIT_BRANCH

ENV GIT_URL=${GIT_URL}
ENV GIT_BRANCH=${GIT_BRANCH}

# VNC Config
RUN mkdir -p ~/.vnc/
RUN echo ${VNC_PASSWORD} | vncpasswd -f > ~/.vnc/passwd
RUN chmod 0600 ~/.vnc/passwd
RUN openssl req -x509 -nodes -newkey rsa:2048 -keyout ~/novnc.pem -out ~/novnc.pem -days 3650 -subj "/C=US/ST=NY/L=NY/O=NY/OU=NY/CN=NY emailAddress=email@example.com"

# Install 42's dependencies
RUN apt-get update && \
    apt-get -y install libglu1-mesa-dev freeglut3-dev mesa-common-dev libglfw3-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# 42's MonteCarlo dependencies
RUN apt-get update && \
    apt-get -y install octave tshark && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# install Julia
RUN  curl -fsSL https://install.julialang.org | sh -s -- --yes

# Clone the 42 repo for a specific branch

RUN mkdir -p /opt/nasa-itc && \
  cd /opt/nasa-itc && \
  git clone --recurse-submodules -b ${GIT_BRANCH} -j4 ${GIT_URL} && \
  cd ./42 &&\
  make clean && \
  make -j7

COPY entrypoint.sh /entrypoint.sh

COPY startapp.sh /startapp.sh

CMD ["/entrypoint.sh"]
