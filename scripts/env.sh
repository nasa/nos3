#!/bin/bash
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR` && pwd)
FSW_DIR=$BASE_DIR/fsw/build/exe/cpu1
GSW_BIN=$BASE_DIR/gsw/cosmos/build/openc3-cosmos-nos3
GSW_DIR=$BASE_DIR/gsw/cosmos
SIM_DIR=$BASE_DIR/sims/build
SIM_BIN=$SIM_DIR/bin
COMPONENT_DIR=$SCRIPT_DIR/../components

if [ -d $SIM_DIR/bin ]; then
    SIMS=$(ls $SIM_BIN/nos3*simulator) 
fi 

DATE=$(date "+%Y%m%d%H%M")
NUM_CPUS="$( nproc )"

USERDIR=$(cd ~/ && pwd)
USER_NOS3_DIR=$(cd ~/ && pwd)/.nos3
USER_FPRIME_PATH=$USERDIR/.cookiecutter_replay
USER_YAMCS_PATH=$USER_NOS3_DIR/.m2
OPENC3_DIR=$USER_NOS3_DIR/openc3
OPENC3_PATH=$OPENC3_DIR/openc3.sh
OPENC3_CLI="$OPENC3_DIR/openc3.sh cli"
OPENC3_CLIROOT="$OPENC3_DIR/openc3.sh cliroot"

INFLUXDB_DB=ait
INFLUXDB_ADMIN_USER=ait
INFLUXDB_ADMIN_PASSWORD=admin_password

DOCKER_COMPOSE_COMMAND="docker compose"
${DOCKER_COMPOSE_COMMAND} version &> /dev/null
if [ "$?" -ne 0 ]; then
  DOCKER_COMPOSE_COMMAND="docker-compose"
fi

###
### Notes: 
###   Podman and/or Docker on RHEL not yet supported
###
#if [ -f "/etc/redhat-release" ]; then
#    DCALL="docker"
#    DFLAGS="docker run --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u $(id -u $(stat -c '%U' $SCRIPT_DIR/env.sh)):$(getent group $(stat -c '%G' $SCRIPT_DIR/env.sh) | cut -d: -f3)"
#    DFLAGS_CPUS="$DFLAGS --cpus=$NUM_CPUS"
#    DCREATE="docker create --rm -it"
#    DNETWORK="docker network"
#else
    DCALL="docker"
    DFLAGS="docker run --rm -it -v /etc/passwd:/etc/passwd:ro -v /etc/group:/etc/group:ro -u $(id -u $(stat -c '%U' $SCRIPT_DIR/env.sh)):$(getent group $(stat -c '%G' $SCRIPT_DIR/env.sh) | cut -d: -f3)"
    DFLAGS_CPUS="$DFLAGS --cpus=$NUM_CPUS"
    DCREATE="docker create --rm -it"
    DNETWORK="docker network"
#fi

DBOX="ivvitc/nos3-64:20251107"

# Radio Config
RADIO_TX_FSW_PORT=5010
RADIO_RX_FSW_PORT=5011

# CryptoLib Ground Config
CRYPTO_RX_GROUND_PORT=6010
CRYPTO_TX_GROUND_PORT=6011
CRYPTO_TX_RADIO_PORT=8010
CRYPTO_RX_RADIO_PORT=8011

# Debugging
#echo "Script directory = " $SCRIPT_DIR
#echo "Base directory   = " $BASE_DIR
#echo "DFLAGS           = " $DFLAGS
#echo "FSW directory    = " $FSW_DIR
#echo "GSW bin          = " $GSW_BIN
#echo "GSW directory    = " $GSW_DIR
#echo "Sim directory    = " $SIM_BIN
#echo "Sim list         = " $SIMS
#echo "Docker flags     = " $DFLAGS
#echo "Docker create    = " $DCREATE
#echo "Docker network   = " $DNETWORK
#echo "Date             = " $DATE
#echo "Local user .nos3 = " $USER_NOS3_DIR
#echo "OpenC3 directory = " $OPENC3_DIR
#echo "OpenC3 path      = " $OPENC3_PATH
