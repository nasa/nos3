#! /usr/bin/env bash

MISSIONS=(m01 m02)
SC=(sc01 sc02)

for MISSION in "${MISSIONS[@]}"
do
  for SPACECRAFT in "${SC[@]}"
  do
    echo $MISSION $SPACECRAFT
    task k8s:delete:kustomization MISSION=${MISSION} SPACECRAFT=${SPACECRAFT} K8S_REPLICAS=1 &
  done
done