#!/bin/bash -e

RECREATE=${1:-no}     # recreate namespaces, pods, etc.
PROVIDER=${2:-podman} # 

MY_PWD=${PWD}

#source ./scripts/functions/deployment.sh

#------------------------------------------------------------------------------
# A script to generate k8s deployment, service... yaml files and apply them
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# TODO: figure out podman and arm64 for nos3
export KIND_EXPERIMENTAL_PROVIDER=${PROVIDER}

K8S_CONTEXT=docker-desktop
NODE_SELECTOR=docker-desktop

K8S_MODE='docker-desktop' # if this value is different from K8S_CONTEXT, target remote k8s on aws

# kubectl --context kind-myorg-dev-missions-exp  apply -k .
#------------------------------------------------------------------------------
NOS3_CONFIG=$(cat ./scripts/nos3.yaml  | yq 'explode(.)' | grep -iv '^null$')
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Mission/SC/Component Arrays
#------------------------------------------------------------------------------
OU=$(echo "${NOS3_CONFIG}" | yq '.metadata.OU')
ENVIRO=$(echo "${NOS3_CONFIG}" | yq '.metadata.ENVIRO')
TENANT=$(echo "${NOS3_CONFIG}" | yq '.metadata.TENANT')
PURPOSE=$(echo "${NOS3_CONFIG}" | yq '.metadata.PURPOSE')
CONTEXT=$(echo "${NOS3_CONFIG}" | yq '.metadata.CONTEXT')${PURPOSE} # -exp must exist to match remote cluster system

REALM=${OU}-${ENVIRO}-${CONTEXT}

nodeSelector=$(echo "${NOS3_CONFIG}" | yq '.defaults.spec.nodes.nodeSelector') # will be overridden if target k8s is remote
imagePullPolicy=$(echo "${NOS3_CONFIG}" | yq '.defaults.spec.nodes.imagePullPolicy')
HEALTHCHECK_PORT=$(echo "${NOS3_CONFIG}" | yq '.defaults.spec.HEALTHCHECK_PORT')

MISSIONS=($(echo "$NOS3_CONFIG" | yq '.missions[] | keys[]'))
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Creds
#------------------------------------------------------------------------------
MISSION_GIT_USER=$(cat ~/.appdat/personal_access_token)
MISSION_GIT_TOKEN=$(cat ~/.appdat/personal_access_token)
MISSION_GIT_SERVER=appdat.jsc.nasa.gov

K8S_LOCAL=true  # to know if k8s is local
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# K8s local or remote
#------------------------------------------------------------------------------
if [ "${K8S_MODE}" != "${K8S_CONTEXT}" ]; then

  AWS_CLUSTER_NAME=${OU}-${ENVIRO}-${CONTEXT}

  ENVIRO=$(echo ${AWS_CLUSTER_NAME} | cut -d- -f2)

  # All this is to capitalize the first letter!
  ENVIRO_USE=`echo ${ENVIRO:0:1} | tr  '[a-z]' '[A-Z]'`${ENVIRO:1}

  echo; echo "Cluster name: $AWS_CLUSTER_NAME"

  mkdir -p /tmp/${HOME}/development/APPDAT/appdat.jsc.nasa.gov/ssmo/SSMO-APPDAT/
  cd /tmp/${HOME}/development/APPDAT/appdat.jsc.nasa.gov/ssmo/SSMO-APPDAT/

  git clone git@appdat.jsc.nasa.gov:ssmo/SSMO-APPDAT/sysconf.git || true
  cd ./sysconf/admin

  make kubeconfig AWS_CLUSTER_NAME=${AWS_CLUSTER_NAME}
  source /tmp/aws-credentials_SSMO-Appdat-Hybrid-Gov${ENVIRO_USE}.sh

  cd ${MY_PWD} # critical

  K8S_CONTEXT=arn:aws-us-gov:eks:${AWS_REGION}:${AWS_ACCOUNT}:cluster/${AWS_CLUSTER_NAME}

  imagePullPolicy=Always
  K8S_LOCAL=false
fi

echo; echo Using and setting context ${K8S_CONTEXT}...
kubectl config use-context ${K8S_CONTEXT}
kubectl config set-context ${K8S_CONTEXT}
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# Image URI with its creds
#------------------------------------------------------------------------------
# TODO: put these in one file
IMAGE_REGISTRY=registry.appdat.jsc.nasa.gov
#IMAGE_URI=${IMAGE_REGISTRY}/ssmo/images/ssmo/nos3/nos3-base:20250217

IMAGE_REGISTRY_USER=${USER}
IMAGE_REGISTRY_PASSWORD=$(cat ~/.appdat/images/SSMO_IMAGES)

IMAGE_SECRET_NAME=appdat-registry 

cat <<-EOF > /tmp/config.json
{
  "auths": {
    "${IMAGE_REGISTRY}": {
      "auth": "$(echo -n ${IMAGE_REGISTRY_USER}:${IMAGE_REGISTRY_PASSWORD} | base64 | tr -d '\n')",
      "username": "${IMAGE_REGISTRY_USER}",
      "password": "${IMAGE_REGISTRY_PASSWORD}"
    }
  }
}
EOF

#------------------------------------------------------------------------------
# Missions Loop
#------------------------------------------------------------------------------
for MISSION in "${MISSIONS[@]}"
do

  _MISSION_ENABLED=$(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.enabled"  | grep -iv '^null$')

  if [ "${_MISSION_ENABLED}" != "true" ]; then
    echo; echo MISSION ${MISSION} is not enabled, add enabled: true, skipping...
    continue
  else
    echo; echo MISSION ${MISSION} is enabled continuing.
  fi

  MISSION_PATH=./environments/${OU}/${ENVIRO}/${CONTEXT}/${MISSION}
  mkdir -p ${MISSION_PATH}

  KUSTOMIZATION_FILE_MISSION=${MISSION_PATH}/kustomization.yaml
  touch ${KUSTOMIZATION_FILE_MISSION}

  # Printout preamble of kustomization file
  cat <<-EOF > ${KUSTOMIZATION_FILE_MISSION}
---
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
resources:
EOF

  SPACECRAFT=($(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[] | keys[]"))

  #------------------------------------------------------------------------------
  # Generate deployment and service yaml files
  #  run is below that
  #------------------------------------------------------------------------------
  for SC in "${SPACECRAFT[@]}"
  do
  
    _SPACECRAFT_ENABLED=$(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[].${SC}.enabled"  | grep -iv '^null$')

    if [ "${_SPACECRAFT_ENABLED}" != "true" ]; then
      echo "  MISSION ${MISSION} is enabled, but SPACECRAFT ${SC} is not enabled, add enabled: true, skipping..."
      continue
    else
      echo "  MISSION ${MISSION} is enabled, and SPACECRAFT ${SC} is enabled continuing."
    fi

    if [ "${K8S_LOCAL}" != "true" ]; then
      nodeSelector=$(
      cat <<EOF
      nodeSelector:
        node-selector: ${MISSION}-${SC}   
EOF
      )
    fi

    K8S_NAMESPACE=${OU}-${ENVIRO}-${CONTEXT}-${MISSION}-${SC}

    cat <<-EOF >> ${KUSTOMIZATION_FILE_MISSION}
  - ./${SC}/kubernetes
EOF
echo "    Created ${KUSTOMIZATION_FILE_MISSION}"

    SPACECRAFT_PATH=${MISSION_PATH}/${SC}
    mkdir -p ${SPACECRAFT_PATH}

    SPACECRAFT_PATH_K8S=${SPACECRAFT_PATH}/kubernetes
    mkdir -p ${SPACECRAFT_PATH_K8S}

    KUSTOMIZATION_FILE_SC=${SPACECRAFT_PATH_K8S}/kustomization.yaml
    rm -f ${KUSTOMIZATION_FILE_SC}
    touch ${KUSTOMIZATION_FILE_SC}

    SPACECRAFT_PATH_DOCKER=${SPACECRAFT_PATH}/docker/${K8S_NAMESPACE}
    mkdir -p ${SPACECRAFT_PATH_DOCKER}

    #------------------------------------------------------------------------------
    # docker-compose
    #------------------------------------------------------------------------------
    DOCKER_COMPOSE_FILE=${SPACECRAFT_PATH_DOCKER}/${K8S_NAMESPACE}_docker-compose.yaml

    rm -rf ${DOCKER_COMPOSE_FILE} 
    touch ${DOCKER_COMPOSE_FILE}

    cat <<-EOF > ${DOCKER_COMPOSE_FILE}
# generated on $(date -u "+%YT%T")

services:
EOF
echo "    Created ${DOCKER_COMPOSE_FILE}"
    #------------------------------------------------------------------------------

    # create configMap per namespace TODO: how to use
CONFIGMAP_FILE=${SPACECRAFT_PATH_K8S}/${K8S_NAMESPACE}_configmap.yaml

    cat <<-EOF > ${CONFIGMAP_FILE}
apiVersion: v1
kind: ConfigMap
metadata:
  name: ${K8S_NAMESPACE}-configmap
  namespace: ${K8S_NAMESPACE}
data:
  key1: value1
EOF
echo "    Created ${CONFIGMAP_FILE}.yaml"

    # Print preamble of kustomization file
    cat <<-EOF > ${KUSTOMIZATION_FILE_SC}
---
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
resources:
EOF
echo "    Created blank ${KUSTOMIZATION_FILE_SC}"

  #------------------------------------------------------------------------------
  # By component
  #------------------------------------------------------------------------------
  COMPONENTS=($(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[].${SC}.components" | grep -iv '^null$' | yq ' keys[]'))

  THINGS=(${COMPONENTS[@]})

  _COMPONENTS=$(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[].${SC}.components" | grep -iv '^null$')

  for COMPONENT in "${THINGS[@]}"
  do

    COMPONENTS=($(echo "${_COMPONENTS}" | yq ' keys[]'))

    _CONTAINER=$(echo "${_COMPONENTS}" | yq ".${COMPONENT}.container" | grep -iv '^null$')

    # TODO: get replicas: value
#    REPLICAS=$(echo "${_COMPONENTS}" | yq ".${COMPONENT}.container.replicas" | grep -iv '^null$')

    REPLICAS=1
    CONTAINER_RESOURCES=$(echo "${_CONTAINER}" | yq ".resources"  | grep -iv '^null$')

    CONTAINER_LIMITS_MEMORY=$(echo "${CONTAINER_RESOURCES}"| yq ".limits.memory"  | grep -iv '^null$' )
    CONTAINER_LIMITS_CPU=$(echo "${CONTAINER_RESOURCES}"| yq ".limits.cpu"  | grep -iv '^null$' )

    CONTAINER_REQUESTS_MEMORY=$(echo "${CONTAINER_RESOURCES}"| yq ".requests.memory"  | grep -iv '^null$' )
    CONTAINER_REQUESTS_CPU=$(echo "${CONTAINER_RESOURCES}"| yq ".requests.cpu"  | grep -iv '^null$' )

    _PATHS=$(echo "${_CONTAINER}" | yq ".PATHS"  | grep -iv '^null$')
    CONTAINER_PATHS=($(echo "${_PATHS}"))

    BASE_DIR=$(echo "${_PATHS}" | yq '.BASE_DIR')
    SIM_BIN=$(echo "${_PATHS}" | yq '.SIM_BIN')
    SIMULATOR_BIN=$(echo "${_PATHS}" | yq '.SIMULATOR_BIN')
    LOG_CONFIG=$(echo "${_PATHS}" | yq '.LOG_CONFIG')

# TODO: note SC_CFG_FILE below
    CFG_FILE=$(echo "${_PATHS}" | yq '.SC_CFG_FILE')
# TODO: TBD wrt to GND
    GND_CFG_FILE=$(echo "${_PATHS}" | yq '.GND_CFG_FILE')

    IMAGE_URI=$(echo "${_PATHS}" | yq '.IMAGE_URI')

    METADATA_NAME=${MISSION}-${SC}-${COMPONENT}
    DEPLOYMENT=${MISSION}-${SC}-${COMPONENT}

    COMPONENT_PATH=${SPACECRAFT_PATH_K8S}/${COMPONENT}
    mkdir -p ${COMPONENT_PATH}

    KUSTOMIZATION_FILE_COMPONENT=${COMPONENT_PATH}/kustomization.yaml
    rm -f ${KUSTOMIZATION_FILE_COMPONENT}

    #------------------------------------------------------------------------------
    # Generate Kustomization files
    #------------------------------------------------------------------------------
    cat <<-EOF >> ${KUSTOMIZATION_FILE_COMPONENT}
---
apiVersion: kustomize.config.k8s.io/v1beta1
kind: Kustomization
resources:
  - ./deployment.yaml
  - ./service.yaml
EOF
echo "      Created ${KUSTOMIZATION_FILE_COMPONENT}"

    SERVICE_FILE_COMPONENT=${COMPONENT_PATH}/service.yaml
    rm -f ${SERVICE_FILE_COMPONENT}

    cat <<-EOF >> ${KUSTOMIZATION_FILE_SC}
  - ./${COMPONENT}
EOF
echo "        Added component ${COMPONENT} to ${SERVICE_FILE_COMPONENT}"

    DEPLOYMENT_FILE_COMPONENT=${COMPONENT_PATH}/deployment.yaml
    rm -f ${DEPLOYMENT_FILE_COMPONENT}

    #------------------------------------------------------------------------------
    # Generate Deployment files
    #------------------------------------------------------------------------------
    cat <<-EOF >> ${DEPLOYMENT_FILE_COMPONENT}
---
apiVersion: apps/v1
kind: Deployment
metadata:
  namespace: ${K8S_NAMESPACE}
  name: ${K8S_NAMESPACE}-${COMPONENT}
  labels:
    app: ${K8S_NAMESPACE}-${COMPONENT}
spec:
  replicas: 0
  selector:
    matchLabels:
      app: ${K8S_NAMESPACE}-${COMPONENT}
  template:
    metadata:
      labels:
        app: ${K8S_NAMESPACE}-${COMPONENT}
    spec:
      imagePullSecrets:
        - name: appdat-registry
${nodeSelector}
      containers:
      - name: ${K8S_NAMESPACE}-${COMPONENT}
        image: ${IMAGE_URI}
        imagePullPolicy: ${imagePullPolicy}
        resources:
          limits:
            memory: ${CONTAINER_LIMITS_MEMORY}
            cpu: ${CONTAINER_LIMITS_CPU}
          requests:
            memory: ${CONTAINER_REQUESTS_MEMORY}
            cpu: ${CONTAINER_REQUESTS_CPU}
        ports:
        - containerPort: ${HEALTHCHECK_PORT}
        env:
          - name: HEALTHCHECK_PORT
            value: "${HEALTHCHECK_PORT}"
          - name: OU
            value: ${OU}
          - name: TENANT
            value: ${TENANT}
          - name: ENVIRO
            value: ${ENVIRO}
          - name: CONTEXT
            value: missions
          - name: MISSION
            value: ${MISSION}
          - name: SPACECRAFT
            value: ${SC}
          - name: MISSION_GIT_USER
            value: ${MISSION_GIT_USER}
          - name: MISSION_GIT_TOKEN
            value: ${MISSION_GIT_TOKEN}
          - name: MISSION_GIT_SERVER
            value: ${MISSION_GIT_SERVER}
          - name: BASE_DIR
            value: ${BASE_DIR}
          - name: SIM_BIN
            value: ${SIM_BIN}
          - name: SIMULATOR_BIN
            value: ${SIMULATOR_BIN}
          - name: LOG_CONFIG
            value: ${LOG_CONFIG}
          - name: CFG_FILE
            value: ${CFG_FILE}
          # - name: SC_CFG_FILE
          #   value: ${SC_CFG_FILE}
          # - name: GND_CFG_FILE
          #   value: ${GND_CFG_FILE}
          - name: NETWORK
            value: ${K8S_NAMESPACE}
          - name: COMPONENT_NAME
            value: ${COMPONENT}
EOF
echo "      Created ${DEPLOYMENT_FILE_COMPONENT}"

    #------------------------------------------------------------------------------
    # Generate Service files
    #------------------------------------------------------------------------------
    cat <<-EOF >> ${SERVICE_FILE_COMPONENT}
---
apiVersion: v1
kind: Service
metadata:
  name: ${COMPONENT}
  namespace: ${K8S_NAMESPACE}
spec:
  selector:
    app: ${K8S_NAMESPACE}-${COMPONENT}
  ports:
    - name: healthcheck-tcp
      protocol: TCP
      port: ${HEALTHCHECK_PORT}
      targetPort: ${HEALTHCHECK_PORT}
    - name: healthcheck-udp
      protocol: UDP
      port: ${HEALTHCHECK_PORT}
      targetPort: ${HEALTHCHECK_PORT}
  type: ClusterIP

EOF
echo "      Created ${SERVICE_FILE_COMPONENT}"

    #------------------------------------------------------------------------------
    # docker-compose: services
    #------------------------------------------------------------------------------
    SERVICE_NAME=${K8S_NAMESPACE}-${COMPONENT}

    cat <<-EOF >> ${DOCKER_COMPOSE_FILE}
  ${SERVICE_NAME}:
    image: ${IMAGE_URI}
    container_name: ${SERVICE_NAME}
    hostname: ${SERVICE_NAME}
    environment:
      HEALTHCHECK_PORT: ${HEALTHCHECK_PORT}
      OU: ${OU}
      TENANT: ${TENANT}
      ENVIRO: ${ENVIRO}
      CONTEXT: missions
      MISSION: ${MISSION}
      SPACECRAFT: ${SC}
      MISSION_GIT_USER: ${MISSION_GIT_USER}
      MISSION_GIT_TOKEN: ${MISSION_GIT_TOKEN}
      MISSION_GIT_SERVER: ${MISSION_GIT_SERVER}
      BASE_DIR: ${BASE_DIR}
      SIM_BIN: ${SIM_BIN}
      NOS3_SIMULATOR_BIN: ${SIMULATOR_BIN}
      LOG_CONFIG: ${LOG_CONFIG}
      CFG_FILE: ${CFG_FILE}
      # SC_CFG_FILE: ${SC_CFG_FILE}
      # GND_CFG_FILE: ${GND_CFG_FILE}
      COMPONENT_NAME: ${COMPONENT}
    privileged: true
    networks:
      - ${K8S_NAMESPACE}
EOF

    echo $MISSION $SC $COMPONENT ${SPACECRAFT[@]}

    done # COMPONENT loop

    #------------------------------------------------------------------------------
    # docker-compose completion for sc
    #------------------------------------------------------------------------------
    cat <<-EOF >> ${DOCKER_COMPOSE_FILE}

networks:
  ${K8S_NAMESPACE}:
    driver: bridge

EOF
    #------------------------------------------------------------------------------

  done # SC loop

done # MISSIONS loop

#------------------------------------------------------------------------------
# Run kubectl on created yaml files
#------------------------------------------------------------------------------
# Missions Loop
for MISSION in "${MISSIONS[@]}"
do

  MISSION_PATH=./environments/${OU}/${ENVIRO}/${CONTEXT}/${MISSION}
  
  _MISSION_ENABLED=$(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.enabled"  | grep -iv '^null$')

  if [ "${_MISSION_ENABLED}" != "true" ]; then
    echo; echo MISSION ${MISSION} is not enabled, add enabled: true, skipping...
    continue
  else
    echo; echo MISSION ${MISSION} is enabled continuing.
  fi

  # resource names, to be used with context, cluster, namespace, pods, and services
  RESOURCE_NAME=${REALM}-${MISSION}

  REALM=docker-desktop

  # contexts/clusters
  K8S_CONTEXT=${REALM}
  K8S_CLUSTER=${K8S_CONTEXT}
  K8S_USER=${K8S_CONTEXT}

#  kind delete cluster --name ${K8S_CLUSTER} || true
#  kind create cluster --name ${K8S_CLUSTER} || true

#  K8S_CONTEXT_PREFIX=kind-
  K8S_CONTEXT=${K8S_CONTEXT_PREFIX}${K8S_CONTEXT}

  # set and use context
  kubectl config set-context ${K8S_CONTEXT}
  kubectl config use-context ${K8S_CONTEXT}
  SPACECRAFT=($(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[] | keys[]"))

  for SC in "${SPACECRAFT[@]}"
  do

    _SPACECRAFT_ENABLED=$(echo "${NOS3_CONFIG}" | yq ".missions[].${MISSION}.spacecraft[].${SC}.enabled"  | grep -iv '^null$')

    if [ "${_SPACECRAFT_ENABLED}" != "true" ]; then
      echo "  MISSION ${MISSION} is enabled, but SPACECRAFT ${SC} is not enabled, add enabled: true, skipping..."
      continue
    else
      echo "  MISSION ${MISSION} is enabled, and SPACECRAFT ${SC} is enabled continuing."
    fi

    K8S_NAMESPACE=${OU}-${ENVIRO}-${CONTEXT}-${MISSION}-${SC}

    if [ "${RECREATE}" == "yes" ]; then
      echo; echo Deleting namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
      kubectl delete namespace ${K8S_NAMESPACE} --context ${K8S_CONTEXT} || true
    fi

    echo; echo Creating namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
    kubectl create namespace ${K8S_NAMESPACE} --context ${K8S_CONTEXT} || true

    echo; echo Creating secret ${IMAGE_SECRET_NAME} in namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
    kubectl delete secret ${IMAGE_SECRET_NAME} -n ${K8S_NAMESPACE} || true
    kubectl create secret generic ${IMAGE_SECRET_NAME} \
      --from-file=.dockerconfigjson=/tmp/config.json \
      --type=kubernetes.io/dockerconfigjson \
      -n ${K8S_NAMESPACE}
    kubectl get secret ${IMAGE_SECRET_NAME} --output=yaml -n ${K8S_NAMESPACE}

    SPACECRAFT_PATH=${MISSION_PATH}/${SC}
    SPACECRAFT_PATH_K8S=${SPACECRAFT_PATH}/kubernetes
    
    KUSTOMIZATION_FILE_SC=${SPACECRAFT_PATH_K8S}

    echo; echo Applying ${KUSTOMIZATION_FILE_SC} in context ${K8S_CONTEXT}...
    kubectl apply -k ${KUSTOMIZATION_FILE_SC}
    kubectl get deployments -n ${K8S_NAMESPACE}

    echo; echo Get pods in namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
    kubectl get pods -o wide -n ${K8S_NAMESPACE}

  done # SC

done # MISSIONS
