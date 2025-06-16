#!/bin/bash

# if [ -n "${HEALTHCHECK_PORT}" ]; then

#   if (( ${HEALTHCHECK_PORT} )); then # only if port is a number proceed
#     pkill -f nc ||
#     while true; do (echo -e 'HTTP/1.1 200 OK\r\n'; echo -e "\n\tSuccess: health check port ${HEALTHCHECK_PORT}" ; echo -e "\t$(date -u +%FT%T)\n") | nc -lp ${HEALTHCHECK_PORT}; done &
#   else
#     echo "HEALTHCHECK_PORT variable is set but to a none number; therefore, not starting health check action"
#   fi

# else
#   echo "HEALTHCHECK_PORT variable is not set; therefore, not starting health check action. Optional. Proceeding"
# fi

pkill -f fsw_respawn.sh || true
pkill -f core-cpu1 || true

$SCRIPT_DIR/fsw/fsw_respawn.sh
