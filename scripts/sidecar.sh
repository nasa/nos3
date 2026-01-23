#!/usr/bin/env bash

# Default values
PROTOCOL=http
SERVER=localhost
PORT=8090
INSTANCE=nos3
PROCESSOR=realtime
#COMMAND="/CFS/CMD/TO_ENABLE_OUTPUT"
COMMAND="/CFS/CMD/CFE_ES_NOOP"
TLS_VERIFY=False

#
PIP=/usr/bin/pip3
PYTHON=python3

usage() {
  cat <<-EOF
  
  Usage: $0 [-h] [-d] [-P] [-s] [-p] [-i] [-c] [-t] [-R] [-w]
  
  Purpose: a script to disable, enable links, and issue a command to a Yamcs server's instance on a specific processor.

  Eg.:
    $0 -h
    $0 -d
    $0 -P http -s localhost -p 8090 -i nos3
    $0 -w
    etc.
  
  Required Arguments:

  Options:
    -h | --help       help
    -d | --defaults   use default configurations. Other arguments will be ignored.
    -P | --protocol   <http|https>,       Default: http
    -s | --server     server's address,   Default: localhost
    -p | --port       service's port,     Default: 8090
    -i | --instance   instance on server, Default: nos3
    -c | --command    command to issue,   Default: /CFS/CMD/CFE_ES_NOOP, invoke /CFS/CMD/TO_ENABLE_OUTPUT to enable telemetry outputs
    -t | --tls_verify                     Default: False
    -R | --processor                      Default: realtime
    -w | --write                          Echo default values to stdout

EOF
}

params="$(getopt -o :h,d,P:s:p:i:c:t:R,w -l 'help,defaults,protocol:,server:,port:,instance:,command:,tls_verify:,processor,write' --name "$(basename $0)" -- "$@")"

# Check if getopt encountered an error
if [ $? -ne 0 ]; then
  echo
  echo "ERROR: Invalid option or missing argument: $@" >&2
  usage
  exit 1
fi

if [ $# -eq 0 ]; then
  echo
  echo "ERROR: no arguments provided: $@" >&2
  usage
  exit 1
fi

eval set -- "$params"
unset params

while true; do
  case "$1" in
    -h|--help)
      usage
      exit 0
      ;;
    -d|--defaults)
      echo; echo "[INFO] Will use default values. Other arguments will be ignored."
      shift
      break
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
    -w|--write)
      echo
      echo "[INFO] defaults are ${PROTOCOL}://${SERVER}:${PORT} on instance: $INSTANCE, with command: $COMMAND, using processor: $PROCESSOR"
      echo
      exit
      ;;
    --)
      shift
      break
      ;;
    *)
      echo "Unrecognized option '$1'"
      usage
      exit
      ;;
  esac

done

echo
echo "Attempting to connect to yamcs on:"
echo "  ${PROTOCOL}://${SERVER}:${PORT} on instance: $INSTANCE, with command: $COMMAND, using processor: $PROCESSOR"
echo

#curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:disable
#curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:disable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:disable
#curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/debug-in:disable

sleep 5

#curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-in:enable
#curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/radio-out:enable
curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable

exit_status=$?

if [ $exit_status -eq 0 ]; then

  ${PIP} install --break-system-packages --user --upgrade yamcs-client && \

  ${PYTHON} <<EOF
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

else
  echo
  echo "FAILED: curl -k -X POST ${PROTOCOL}://${SERVER}:${PORT}/api/links/nos3/truth42-in:enable"
  echo
  exit 0
fi