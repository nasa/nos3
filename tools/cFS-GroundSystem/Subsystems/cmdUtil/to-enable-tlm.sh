#!/bin/sh
# to-enable-tlm.sh : This script will call the cmdUtil program with a command packet to the
#                    TO cFE application. It will tell TO to start sending telemetry to 
#                    IP address 192.216.225.238 ( klamath ).

## These are the defaults
HOST=127.0.0.1
CLIENT=127.0.0.1

USAGE="usage is: `basename $0` [-H host ] [-C client]"
set -- `getopt hC:H: $* 2>/dev/null`

while [ $1 != -- ]
do
	case $1 in
	-C)     CLIENT=$2; shift 2;;
	-H)     HOST=$2; shift 2;;
	-h)     echo $USAGE; exit;;
	--)     shift; break;;
	*)      echo "$* illegal option"; shift;;
	esac
done
shift;

./cmdUtil --host=${HOST} --pktid=0x1880 --cmdcode=6 --string="17:${CLIENT}"
