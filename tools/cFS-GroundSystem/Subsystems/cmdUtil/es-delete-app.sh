# es-delete-app.sh : This script will call the cmdUtil program with a command packet to the
#                    ES cFE application. It will tell ES to delete the TO App. It is defaulting to the localhost.
./cmdUtil --pktid=0x1806 --cmdcode=5 --string="20:ES_APP"
