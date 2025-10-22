#! /bin/bash -x

#set -e

_ARGS="$@"

DEPLOYMENT_ENVIRO=${DEPLOYMENT_ENVIRO}

#------------------------------------------------------------------------------------
# source of NOS3, ${PROVIDER_PATH}, ${GIT_SERVER}, ${GIT_PATH}, ${MISSION} are from .env files
#------------------------------------------------------------------------------------
PROVIDER_PATH=${OU}-${ENVIRO}

mkdir -p /${PROVIDER_PATH}
cd /${PROVIDER_PATH}

# To clone mission and spacecraft centric configurations the below 

# variable assignments below are to allow external value to come from sources.
MISSION_GIT_USER=${MISSION_GIT_USER}
MISSION_GIT_TOKEN=${MISSION_GIT_TOKEN}
MISSION_GIT_SERVER=${MISSION_GIT_SERVER}
MISSION_GIT_CREDS=${MISSION_GIT_USER}:${MISSION_GIT_TOKEN}

MISSION_GIT_PATH=${OU}/${CONTEXT}/${TENANT}/${ENVIRO}
MISSION_GIT_URL=https://${MISSION_GIT_CREDS}@${MISSION_GIT_SERVER}/${MISSION_GIT_PATH}/${MISSION}.git

PROVIDER_GIT_PATH=/${PROVIDER_PATH}/git/${MISSION_GIT_SERVER}/${MISSION_GIT_PATH}
PROVIDER_MISSION_PATH=${PROVIDER_GIT_PATH}/${MISSION}

mkdir -p ${PROVIDER_GIT_PATH} && cd ${PROVIDER_GIT_PATH}
# the || true permits the continuation even if it ${MISSION} folder exists, 
# this would happen if path is a mounted docker volume instead.
#git clone ${MISSION_GIT_URL} || true

#cd ${PROVIDER_MISSION_PATH}

# Unique to image
SIM_BIN=${SIM_BIN}

VAR_DIR=${SIM_BIN}/var
RUN_DIR=${VAR_DIR}/run

SIMULATOR_BIN=${SIMULATOR_BIN}

CFG_FILE=${CFG_FILE}
LOG_CONFIG=${LOG_CONFIG}
COMPONENT_NAME=${COMPONENT_NAME}

#------------------------------------------------------------------------------------
# run nos3 simulator in the background and capture pid
#------------------------------------------------------------------------------------
mkdir -p ${RUN_DIR}

pkill -f ${SIMULATOR_BIN} || true

${SIMULATOR_BIN} --config-file ${CFG_FILE} --log-config-file ${LOG_CONFIG} ${COMPONENT_NAME} &
echo $! > ${RUN_DIR}/pid
#------------------------------------------------------------------------------------

# healthcheck on port ${HEALTHCHECK_PORT}, if set
HEALTHCHECK_PORT=${HEALTHCHECK_PORT} # environment variable

if [ -n "${HEALTHCHECK_PORT}" ]; then

  if (( ${HEALTHCHECK_PORT} )); then # only if port is a number proceed
    pkill -f nc ||
    while true; do (echo -e 'HTTP/1.1 200 OK\r\n'; echo -e "\n\tSuccess: health check port ${HEALTHCHECK_PORT}" ; echo -e "\t$(date -u +%FT%T)\n") | nc -lp ${HEALTHCHECK_PORT}; done &
  else
    echo "HEALTHCHECK_PORT variable is set but to a none number; therefore, not starting health check action"
  fi

else
  echo "HEALTHCHECK_PORT variable is not set; therefore, not starting health check action. Optional. Proceeding"
fi

tail -f /dev/null