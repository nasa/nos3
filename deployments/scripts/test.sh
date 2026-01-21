#! /usr/bin/env bash

PROJECT=nos3
MISSIONS=(m01 m02)
SC=(sc01 sc02)
FORTYTWO_HOST_PORTS=(30090 30091)
YAMCS_HOST_PORTS=(8090 8091)
OPENMCT_HOST_PORTS=(9000 9001)

for MISSION in "${MISSIONS[@]}"
do

  for SPACECRAFT in "${SC[@]}"
  do

    for FORTYTWO_HOST_PORT in "${FORTYTWO_HOST_PORTS[@]}"
    do

      for YAMCS_HOST_PORT in "${YAMCS_HOST_PORTS[@]}"
      do

        for OPENMCT_HOST_PORT in "${OPENMCT_HOST_PORTS[@]}"
        do

          task generate:env \
            PROJECT=${PROJECT} \
            FLEET=${PROJECT} \
            MISSION=${MISSION} \
            SPACECRAFT=${SPACECRAFT} \
            FORTYTWO_HOST_PORT=${FORTYTWO_HOST_PORT} \
            YAMCS_HOST_PORT=${YAMCS_HOST_PORT} \
            OPENMCT_HOST_PORT=${OPENMCT_HOST_PORT}

        done
      done
    done
  done
done

