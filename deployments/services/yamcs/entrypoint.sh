#!/bin/bash

pkill -9 java
rm -rf ${YAMCS_DATA_DIR}/_global.rdb/LOCK
rm -rf ${YAMCS_DATA_DIR}/nos3.rdb/LOCK

mkdir -p /tmp/nos3
mkdir -p /tmp/nos3/data
mkdir -p /tmp/nos3/data/cam
mkdir -p /tmp/nos3/data/evs
mkdir -p /tmp/nos3/data/hk
mkdir -p /tmp/nos3/data/inst
mkdir -p /tmp/nos3/uplink

# Start YAMCS server
mvn ${MAVEN_HTTPS_PROXY} -Dmaven.repo.local=${MAVEN_REPO_LOCAL} -DCOMPONENT_DIR=${COMPONENT_DIR} yamcs:run

sleep 3

curl -X POST http://localhost:8090/api/links/nos3/radio-out:disable
sleep 1
curl -X POST http://localhost:8090/api/links/nos3/radio-out:enable

curl -X POST http://localhost:8090/api/links/nos3/truth42-in:disable
sleep 1
curl -X POST http://localhost:8090/api/links/nos3/truth42-in:enable

tail -f /dev/null
