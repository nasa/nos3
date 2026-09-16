
REALM=myorg-dev-missions-exp
K8S_CONTEXT=kind-${REALM}

K8S_CONTEXT=docker-desktop
K8S_NAMESPACE=${REALM}-m01-sc1

COMPONENT=fortytwo

echo; echo Creating namespace ${K8S_NAMESPACE} in context ${K8S_CONTEXT}...
kubectl create namespace ${K8S_NAMESPACE} --context ${K8S_CONTEXT} || true

kubectl --context ${K8S_CONTEXT}  apply -k .

K8S_POD=$(kubectl get pods --context ${K8S_CONTEXT} --namespace ${K8S_NAMESPACE} | grep ${COMPONENT} | cut -d' ' -f1)

x=${HOME}/development/github.com/haisamido/nos3/cfg/InOut

kubectl --context ${K8S_CONTEXT} --namespace ${K8S_NAMESPACE} cp ${x} ${K8S_POD}:/42/X 

# K8S_POD=myorg-dev-missions-exp-m01-sc1-fortytwo-7dcfdc6446-z96md
# K8S_CONTEXT=kind-myorg-dev-missions-exp
# K8S_NAMESPACE=myorg-dev-missions-exp-m01-sc1

sleep 10
kubectl port-forward pod/${K8S_POD} 30003:80   --context ${K8S_CONTEXT}   --namespace ${K8S_NAMESPACE}
