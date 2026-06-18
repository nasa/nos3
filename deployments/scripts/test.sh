#! /usr/bin/env bash

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms1 FORTYTWO_HOST_PORT=30090 YAMCS_HOST_PORT=8090  OPENMCT_HOST_PORT=9000;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091  OPENMCT_HOST_PORT=9001;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms3 FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092  OPENMCT_HOST_PORT=9002;
  task up

# Helm Chart stuff

# Enviro variables if needed
export K8S_CONTEXT=docker-desktop

# Build necessary images once
task k8s:helm:build

# Install Specific mission-spacecraft in parallel
task k8s:helm:install K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 FORTYTWO_HOST_PORT=40091 YAMCS_HOST_PORT=18091 OPENMCT_HOST_PORT=19001 &
task k8s:helm:install K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=40092 YAMCS_HOST_PORT=18092 OPENMCT_HOST_PORT=19002 &
wait

# Uninstall Specific mission-spacecraft in parallel
task k8s:helm:delete K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 &
task k8s:helm:delete K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 &
wait

# Uninstall Helm charts
task k8s:helm:delete:all K8S_CONTEXT=${K8S_CONTEXT} 
wait

# Port-forward all services:
task k8s:helm:port-forward K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 FORTYTWO_HOST_PORT=40091 YAMCS_HOST_PORT=18091 OPENMCT_HOST_PORT=19001 &
wait

task k8s:helm:port-forward K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=40092 YAMCS_HOST_PORT=18092 OPENMCT_HOST_PORT=19002 &
wait

# List all Port-forward services:
task k8s:helm:port-forward:list K8S_CONTEXT=${K8S_CONTEXT}

# Kill port-forward for specific services:
task k8s:helm:port-forward:kill K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 &
task k8s:helm:port-forward:kill K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 &
wait

# Kill all port-forwards
task k8s:helm:port-forward:kill:all K8S_CONTEXT=${K8S_CONTEXT} 


exit 0

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
        #&& task up

    done

  done

done

echo "Environment files generated successfully."
