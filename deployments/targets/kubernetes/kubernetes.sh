
REALM=nos3
#K8S_CONTEXT=kind-${REALM}

# kompose convert --file ../../docker/docker-compose.yaml 

export K8S_CONTEXT=docker-desktop
export K8S_NAMESPACE=${REALM}-m01-sc01

COMPONENT=fortytwo

echo; echo Creating namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
kubectl create namespace ${K8S_NAMESPACE} --context ${K8S_CONTEXT} || true

kubectl --context ${K8S_CONTEXT} --namespace ${K8S_NAMESPACE} apply -k . 

echo
echo "Waiting on fortytwo pod to be ready"
echo
sleep 10

K8S_POD=$(kubectl get pods --context ${K8S_CONTEXT} --namespace ${K8S_NAMESPACE} | grep ${COMPONENT} | cut -d' ' -f1)

x=${HOME}/development/github.com/haisamido/nos3/cfg/InOut

kubectl --context ${K8S_CONTEXT} --namespace ${K8S_NAMESPACE} cp ${x} ${K8S_POD}:/opt/nasa-itc/42/X 

sleep 10
kubectl port-forward pod/${K8S_POD} 30003:80   --context ${K8S_CONTEXT}   --namespace ${K8S_NAMESPACE}
