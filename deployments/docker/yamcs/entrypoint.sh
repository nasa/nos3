#!/bin/bash

rm -rf ${YAMCS_DATA_DIR}/_global.rdb/LOCK

mvn -Dmaven.repo.local=${MVN_REPO_LOCAL} -DCOMPONENT_DIR=${COMPONENT_DIR} yamcs:run

tail -f /dev/null
