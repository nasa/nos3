#!/bin/bash -i
#
# Convenience script for NOS3 development
#

FLIGHT_SOFTWARE="cFS"

read_xml ()
{
    local IFS=\>
    read -d \< ENTITY CONTENT
    local ret=$?
    TAG_NAME=${ENTITY%% *}
    ATTRIBUTES=${ENTITY#* }
    return $ret
}

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
source $SCRIPT_DIR/env.sh
echo ""
echo ""

echo "Create local user directory..."
mkdir $USER_NOS3_DIR 2> /dev/null
echo "  "$USER_NOS3_DIR
mkdir $USER_NOS3_DIR/42 2> /dev/null
echo ""
echo ""

# while read_xml; do
#     if [[ $ENTITY = "fsw" ]]; then 
#         FLIGHT_SOFTWARE=$CONTENT
#         break
#     fi
# done < ./cfg/nos3-mission.xml

# echo "Using Flight Software: ${FLIGHT_SOFTWARE}"
# echo ""
# echo ""

# if [[ $FLIGHT_SOFTWARE = "fprime" ]] ; then

    echo "Preparing FPrime FSW"
    echo $DFLAGS_CPUS $DBOX
    $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR --name "fprime_prepare" -w $BASE_DIR $DBOX ./scripts/prep_fprime.sh

    echo ""
    echo ""

#     echo "Prepare nos3 docker container..."
#     $DCALL image pull $DBOX
#     echo ""
#     echo ""

#     echo "Prepare 42..."
#     cd $USER_NOS3_DIR
#     git clone https://github.com/nasa-itc/42.git --depth 1 -b nos3-main
#     cd $USER_NOS3_DIR/42
#     $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -w $USER_NOS3_DIR/42 --name "nos3_42_build" $DBOX make
#     echo ""
#     echo ""
# fi   

# if [[ $FLIGHT_SOFTWARE = "cfs" ]] ; then
    echo "Clone openc3-cosmos into local user directory..."
    cd $USER_NOS3_DIR
    git clone https://github.com/nasa-itc/openc3-nos3.git --depth 1 -b main $USER_NOS3_DIR/cosmos
    git reset --hard
    echo ""
    echo ""

    echo "Prepare cosmos docker container..."
    $DCALL image pull ballaerospace/cosmos:4.5.0
    echo ""
    echo ""

    echo "Prepare nos3 docker container..."
    $DCALL image pull $DBOX
    echo ""
    echo ""

    echo "Prepare 42..."
    cd $USER_NOS3_DIR
    git clone https://github.com/nasa-itc/42.git --depth 1 -b nos3-main
    cd $USER_NOS3_DIR/42
    $DFLAGS_CPUS -v $BASE_DIR:$BASE_DIR -v $USER_NOS3_DIR:$USER_NOS3_DIR -w $USER_NOS3_DIR/42 --name "nos3_42_build" $DBOX make
    echo ""
    echo ""
# fi
