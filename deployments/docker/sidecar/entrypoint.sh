#!/bin/bash -x

SERVER=$1
SERVER=${SERVER:-localhost}

PORT=$2
PORT=${PORT:-8090}

COMMAND=$3
COMMAND=${COMMAND:-/CFS/CMD/TO_ENABLE_OUTPUT}

INSTANCE=nos3

curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-in:disable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-out:disable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/truth42-in:disable

sleep 5

curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-in:enable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/radio-out:enable
curl -X POST http://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable

pip3 install --upgrade yamcs-client 

#python3 /commanding.py ${SERVER} ${PORT}

python3 <<EOF
import sys

from yamcs.client import YamcsClient

SERVER='${SERVER}'
PORT=${PORT}
INSTANCE='${INSTANCE}'

client = YamcsClient(f"{SERVER}:{PORT}")
processor = client.get_processor(instance='${INSTANCE}', processor='realtime')

command_name = "${COMMAND}"
arguments = {} # Example arguments, NOT RELEVANT FOR NOOP command

command_handle = processor.issue_command(command_name, args=arguments)

print(f"Issued command: {command_handle}")
EOF

if [ -f /.dockerenv ]; then
  tail -f /dev/null
fi

