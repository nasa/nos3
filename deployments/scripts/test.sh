#! /usr/bin/env bash

#set -e

config=$(cat ./scripts/nos3.yaml)

PROJECTS=$(echo "$config" | yq ' .projects | select(.) | keys []')

task test PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms3 FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092  OPENMCT_HOST_PORT=9002;

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms1 FORTYTWO_HOST_PORT=30090 YAMCS_HOST_PORT=8090  OPENMCT_HOST_PORT=9000;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091  OPENMCT_HOST_PORT=9001;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms3 FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092  OPENMCT_HOST_PORT=9002;


task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms4 FORTYTWO_HOST_PORT=30093 YAMCS_HOST_PORT=8093  OPENMCT_HOST_PORT=9003
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms5 FORTYTWO_HOST_PORT=30094 YAMCS_HOST_PORT=8094  OPENMCT_HOST_PORT=9004
  task up

PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091  OPENMCT_HOST_PORT=9001 task up

# task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms4 FORTYTWO_HOST_PORT=30093 YAMCS_HOST_PORT=8093  OPENMCT_HOST_PORT=9003
#   task up

exit 0

for PROJECT in "${PROJECTS[@]}"
do
  MISSIONS=$(echo "$config" | yq " .projects.${PROJECT}.missions | keys []")

  for MISSION in ${MISSIONS[@]}
  do
    SPACECRAFT=$(echo "$config" | yq ".projects.${PROJECT}.missions.${MISSION}.spacecraft | keys []")

    for SC in ${SPACECRAFT[@]}
    do

      FORTYTWO_HOST_PORT=$(echo "$config" | yq " .projects.${PROJECT}.missions.${MISSION}.spacecraft.${SC}.components.fortytwo.port // \"default\"")
      YAMCS_HOST_PORT=$(echo "$config" | yq " .projects.${PROJECT}.missions.${MISSION}.spacecraft.${SC}.components.yamcs.port // \"default\"")
      OPENMCT_HOST_PORT=$(echo "$config" | yq " .projects.${PROJECT}.missions.${MISSION}.spacecraft.${SC}.components.openmct.port // \"default\"")

      echo "Generating .env for Project: ${PROJECT}, Mission: ${MISSION}, Spacecraft: ${SC}"

      task generate:env \
        PROJECT=${PROJECT} \
        FLEET=${PROJECT} \
        MISSION=${MISSION} \
        SPACECRAFT=${SC} \
        FORTYTWO_HOST_PORT=${FORTYTWO_HOST_PORT} \
        YAMCS_HOST_PORT=${YAMCS_HOST_PORT} \
        OPENMCT_HOST_PORT=${OPENMCT_HOST_PORT} 
        #&& task up

    done

  done

done

echo "Environment files generated successfully."


# task generate:env PROJECT=nos3 FLEET=nos3 MISSION=m01 SPACECRAFT=sc01 FORTYTWO_HOST_PORT=30090 YAMCS_HOST_PORT=8090  OPENMCT_HOST_PORT=9000 && \
#  task up

# task generate:env PROJECT=nos3 FLEET=nos3 MISSION=m01 SPACECRAFT=sc02 FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091  OPENMCT_HOST_PORT=9001 && \
#   task up

# task generate:env PROJECT=nos3 FLEET=nos3 MISSION=m01 SPACECRAFT=sc03 FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092  OPENMCT_HOST_PORT=9002 && \
#   task up

# task generate:env PROJECT=ssmo FLEET=ssmo MISSION=m02 SPACECRAFT=sc01 FORTYTWO_HOST_PORT=30093 YAMCS_HOST_PORT=8093  OPENMCT_HOST_PORT=9003 && \
#   task up
