#!/usr/bin/env bash

pkill -f node
pkill -f npm

npm start

tail -f /dev/null
