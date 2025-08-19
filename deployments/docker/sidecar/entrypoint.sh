#!/bin/bash -x

SERVER=$1
SERVER=${SERVER:-active-gs}
PORT=$2
PORT=${PORT:-8090}

curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-in:disable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-out:disable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/truth42-in:disable

sleep 5

curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-in:enable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-out:enable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable

pip3 install --upgrade yamcs-client && \
    python3 /commanding.py ${SERVER} ${PORT}

tail -f /dev/null
