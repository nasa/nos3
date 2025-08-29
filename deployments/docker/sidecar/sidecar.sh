#!/bin/bash

usage() {

  cat <<-EOF
  
  Usage: $0 [-h] [-P] [-s] [-p] [-i] [-c] [-t] [-R]
  
  Eg.:
    $0 -h
    $0 -P http -s localhost -p 8090 -i nos3 
    etc.
  
  Required Arguments:

  Options:
   -h | --help       help
   -P | --protocol   <http|https>,       Default: http
   -s | --server     server's address,   Default: localhost
   -p | --port       service's port,     Default: 8090
   -i | --instance   instance on server, Default: nos3
   -c | --command    command to issue,   Default: /CFS/CMD/TO_ENABLE_OUTPUT
   -t | --tls_verify                     Default: False
   -R | --processor                      Default: realtime

EOF


}

OPTS=$(getopt --options h,P:s:p:i:c:t:R --longoptions 'help,protocol:,server:,port:,instance:,command:,tls_verify:,processor' -n "$(basename $0)" -- $@)

eval set -- "$OPTS"

# Defaults
PROTOCOL=http
SERVER=localhost
PORT=8090
INSTANCE=nos3
PROCESSOR=realtime
COMMAND="/CFS/CMD/TO_ENABLE_OUTPUT"
TLS_VERIFY=False

while true; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    -P|--protocol)
      PROTOCOL="$2"
      shift 2
      ;;      
    -s|--server)
      SERVER="$2"
      shift 2
      ;;
    -p|--port)
      PORT="$2"
      shift 2
      ;;
    -i|--instance)
      INSTANCE="$2"
      shift 2
      ;;
    -c|--command)
      COMMAND="$2"
      shift 2
      ;;
    -t|--tls_verify)
      TLS_VERIFY="$2"
      shift 2
      ;;
    --)
      break
      ;;
    *)
      echo "Unrecognized option '$1'"
      usage
      ;;
  esac

done

echo $PROTOCOL
echo $SERVER
echo $PORT
echo $INSTANCE
echo $COMMAND
echo $TLS_VERIFY
echo $PROCESSOR

curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:disable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:disable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:disable

sleep 5

curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:enable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:enable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable

pip3 install --break-system-packages --user --upgrade yamcs-client && \

python3 <<EOF
import sys

from yamcs.client import YamcsClient

SERVER='${SERVER}'
PORT=${PORT}
INSTANCE='${INSTANCE}'
PROTOCOL='${PROTOCOL}'
COMMAND='${COMMAND}'
TLS_VERIFY='${TLS_VERIFY}'
PROCESSOR='${PROCESSOR}'

client = YamcsClient(f"{PROTOCOL}://{SERVER}:{PORT}", tls_verify=f"{TLS_VERIFY}")
processor = client.get_processor(instance=f"{INSTANCE}", processor=f"{PROCESSOR}")

command_name = f"{COMMAND}"
arguments = {} # Example arguments, NOT RELEVANT FOR NOOP command

command_handle = processor.issue_command(command_name, args=arguments)

print(f"Issued command: {command_handle}")
EOF
