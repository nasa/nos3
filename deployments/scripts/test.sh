#! /usr/bin/env bash

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms1 FORTYTWO_HOST_PORT=30090 YAMCS_HOST_PORT=8090  OPENMCT_HOST_PORT=9000;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms2 FORTYTWO_HOST_PORT=30091 YAMCS_HOST_PORT=8091  OPENMCT_HOST_PORT=9001;
  task up

task env:generate PROJECT=ssmo FLEET=ssmo MISSION=mms SPACECRAFT=mms3 FORTYTWO_HOST_PORT=30092 YAMCS_HOST_PORT=8092  OPENMCT_HOST_PORT=9002;
  task up

# Helm Chart stuff

# Enviro variables if needed
cd ./deployments

# Build necessary images once
task build

# Note the below varying SPACECRAFT and PORT values

#------------------------------------------------------------------------
#   Docker via Docker Compose for a specific mission-spacecraft
#------------------------------------------------------------------------
task generate:env \
  PROJECT=nos3 MISSION=m01 SPACECRAFT=sc01 \
  FORTYTWO_HOST_PORT=10081 YAMCS_HOST_PORT=18091 OPENMCT_HOST_PORT=19001 && \
  task up

task generate:env \
  PROJECT=nos3 MISSION=m01 SPACECRAFT=sc02 \
  FORTYTWO_HOST_PORT=10082 YAMCS_HOST_PORT=18092 OPENMCT_HOST_PORT=19002 && \
  task up
#------------------------------------------------------------------------

#------------------------------------------------------------------------
#   Deploy NOS3 via K8S Helm Chart for a specific mission-spacecraft in parallel
#------------------------------------------------------------------------
export K8S_CONTEXT=docker-desktop

task k8s:helm:up K8S_CONTEXT=${K8S_CONTEXT} \
  PROJECT=nos3 MISSION=m01 SPACECRAFT=sc01 \
  FORTYTWO_HOST_PORT=20081 YAMCS_HOST_PORT=28091 OPENMCT_HOST_PORT=29001 &

task k8s:helm:up K8S_CONTEXT=${K8S_CONTEXT} \
  PROJECT=nos3 MISSION=m01 SPACECRAFT=sc02 \
  FORTYTWO_HOST_PORT=20082 YAMCS_HOST_PORT=28092 OPENMCT_HOST_PORT=29002 &
#------------------------------------------------------------------------

#------------------------------------------------------------------------
# List all port-forward services in K8S_CONTEXT
#------------------------------------------------------------------------
task k8s:helm:port-forward:list K8S_CONTEXT=${K8S_CONTEXT}

#------------------------------------------------------------------------
# Kill port-forward for specific spacecraft in K8S_CONTEXT
#------------------------------------------------------------------------
task k8s:helm:port-forward:kill K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 &
task k8s:helm:port-forward:kill K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 &
wait

#------------------------------------------------------------------------
# Kill all port-forwards to K8S_CONTEXT
#------------------------------------------------------------------------
task k8s:helm:port-forward:kill:all K8S_CONTEXT=${K8S_CONTEXT}

#------------------------------------------------------------------------
# Delete specific mission-spacecraft in parallel in K8S_CONTEXT
#------------------------------------------------------------------------
task k8s:helm:delete K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms1 &
task k8s:helm:delete K8S_CONTEXT=${K8S_CONTEXT} PROJECT=ssmo MISSION=mms SPACECRAFT=mms2 &
wait

#------------------------------------------------------------------------
# Delete all Helm charts in K8S_CONTEXT
#------------------------------------------------------------------------
task k8s:helm:delete:all K8S_CONTEXT=${K8S_CONTEXT}

#------------------------------------------------------------------------
# Restore Environment to defaults in K8S_CONTEXT
#------------------------------------------------------------------------
task env:set:defaults

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
