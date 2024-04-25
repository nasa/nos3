#!/bin/bash -i
#
# Convenience script for NOS3 development
#

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$(cd `dirname $SCRIPT_DIR`/.. && pwd)
DATE=$(date "+%Y%m%d_%H%M")
COSMOS_DIR=$(cd $BASE_DIR/gsw/cosmos && pwd)
COSMOS_TOOLS_DIR=$(cd $COSMOS_DIR/tools && pwd)

cd $COSMOS_TOOLS_DIR

###
### Live Telemetry
###

COSMOS_DATA=$(cd $BASE_DIR/gsw/cosmos/outputs/logs && pwd)
TLM_FILES=$(cd $COSMOS_DATA && ls 20*_tlm.bin)
for entry in $TLM_FILES
do
    #echo "$entry"
    #echo "${entry%.*}"

    ruby TlmExtractor --system nos3_system.txt --config ../../targets/CFS/tools/tlm_extractor/CF_HKPACKET.txt --input $entry --output $COSMOS_DATA"/"${entry%.*}"_CF_HKPACKET.csv" &
    ruby TlmExtractor --system nos3_system.txt --config ../../targets/CFS/tools/tlm_extractor/CFE_EVS_PACKET.txt --input $entry --output $COSMOS_DATA"/"${entry%.*}"_CFE_EVS_PACKET.csv" &
    ruby TlmExtractor --system nos3_system.txt --config ../../targets/CFS/tools/tlm_extractor/LC_HKPACKET.txt --input $entry --output $COSMOS_DATA"/"${entry%.*}"_LC_HKPACKET.csv" &
    ruby TlmExtractor --system nos3_system.txt --config ../../targets/CFS/tools/tlm_extractor/SC_HKTLM.txt --input $entry --output $COSMOS_DATA"/"${entry%.*}"_SC_HKTLM.csv" &

    wait
done

###
### DS Files
###

#HK_DATA=$(cd $COSMOS_DATA/data/hk && pwd)
#HK_FILES=$(cd $HK_DATA && ls *.ds)
#for entry in $HK_FILES
#do
#    ruby TlmExtractor --system nos3_ds_system.txt --config ../../targets/CFS/tools/tlm_extractor/CF_HKPACKET.txt --input ./data/hk/$entry --output $HK_DATA"/"${entry%.*}"_CF_HKPACKET.csv" &
#
#    wait
#done

exit 0
