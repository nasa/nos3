#! /usr/bin/env bash

set -e

config=$(cat ./scripts/nos3.yaml)

PROJECTS=$(echo "$config" | yq ' .projects | select(.) | keys []')

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

    done

  done

done

echo "Environment files generated successfully."