#!/usr/bin/env bash

pkill -f java

mvn ${MAVEN_HTTPS_PROXY} -DskipTests yamcs:run

tail -f /dev/null
