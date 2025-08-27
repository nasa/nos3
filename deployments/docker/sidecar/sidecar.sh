SERVER=$1
SERVER=${SERVER:-localhost}

PORT=$2
PORT=${PORT:-8090}

COMMAND=$3
COMMAND=${COMMAND:-/CFS/CMD/TO_ENABLE_OUTPUT}
INSTANCE=nos3

PROTOCOL=http

curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:disable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:disable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:disable

sleep 5

curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:enable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:enable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable

pip3 install --upgrade yamcs-client && \

python3 <<EOF
import sys

from yamcs.client import YamcsClient

SERVER='${SERVER}'
PORT=${PORT}
INSTANCE='${INSTANCE}'
PROTOCOL='${PROTOCOL}'
COMMAND='${COMMAND}'

client = YamcsClient(f"{PROTOCOL}://{SERVER}:{PORT}", tls_verify=False)
processor = client.get_processor(instance=f"{INSTANCE}", processor='realtime')

command_name = f"{COMMAND}"
arguments = {} # Example arguments, NOT RELEVANT FOR NOOP command

command_handle = processor.issue_command(command_name, args=arguments)

print(f"Issued command: {command_handle}")
EOF
