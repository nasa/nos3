#!/bin/bash -i
#
# Convenience script for NOS3 development
#

# Note the first argument passed is expected to be the BASE_DIR of the NOS3 repository
source $1/scripts/env.sh

export GSW="ait"

echo "AIT launch..."
gnome-terminal --tab --title="AIT" -- $DCALL run --rm -it -v $BASE_DIR:$BASE_DIR -v /tmp/nos3:/tmp/nos3 --name ait -h ait -p 8001:8001 --network=nos3_core ghcr.io/sphinxdefense/gsw-ait:main "source ~/.bashrc && ait-server"
$DCALL run --rm -d --cpus=$NUM_CPUS -h influxdb --name influxdb -p 8086:8086 -e INFLUXDB_DB=$INFLUXDB_DB -e INFLUXDB_ADMIN_USER=$INFLUXDB_ADMIN_USER -e INFLUXDB_ADMIN_PASSWORD=$INFLUXDB_ADMIN_PASSWORD --network=nos3_core influxdb:1.8
$DCALL run --rm -d --cpus=$NUM_CPUS --name ttc-command -p 80:80 --network=nos3_core ghcr.io/sphinxdefense/ttc-command:main
echo ""

pidof firefox > /dev/null
if [ $? -eq 1 ]
then
    echo "Firefox launch..."
    sleep 3
    firefox http://localhost:80 &
    echo ""
fi
