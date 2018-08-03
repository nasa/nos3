#!/bin/bash

# Copyright (C) 2015 - 2016 National Aeronautics and Space Administration. 
# All Foreign Rights are Reserved to the U.S. Government.
#
#   This software is provided "as is" without any warranty of any, kind 
#   either express, implied, or statutory, including, but not
#   limited to, any warranty that the software will conform to, 
#   specifications any implied warranties of merchantability, fitness
#   for a particular purpose, and freedom from infringement, and any 
#   warranty that the documentation will conform to the program, or
#   any warranty that the software will be error free.
#
#   In no event shall NASA be liable for any damages, including, but not 
#   limited to direct, indirect, special or consequential damages,
#   arising out of, resulting from, or in any way connected with the 
#   software or its documentation.  Whether or not based upon warranty,
#   contract, tort or otherwise, and whether or not loss was sustained 
#   from, or arose out of the results of, or use of, the software,
#   documentation or services provided hereunder
#
#   ITC Team
#   NASA IV&V
#   ivv-itc@lists.nasa.gov

usage()
{
	echo "  Usage:  raw_bytes_with_sync_to_packet_lines.sh <sync bytes>  <cosmos raw telemetry file>"
        echo "     <sync bytes> are in the form \"nn nn nn\" where \"nn\" is a two digit hex number, e.g.  \"de ad\""
	echo "     cosmos raw telemetry files come from executing \"start_raw_logging_interface\" in COSMOS"
	exit 1
}

if [ "$#" -ne 2 ]; then
	echo "ERROR:  Must supply two arguments."
	usage
fi

SYNC=$1
FILE=$2

if [ ! -e $2 ]; then
	echo "ERROR:  File $2 does not exist."
	usage
fi

# Here's where the work happens
hexdump -C -v $FILE | cut -c11-33,35-58 | tr '\n' ' ' | sed -e "s/$SYNC/\n$SYNC/g;" # 33 - 10 = 23, 58 - 34 = 24, 23 + 24 = 47
#xxd $FILE | cut -c10-48 | sed -r -e 's/(\S\S)(\S\S)/\1 \2/g;' | tr '\n' ' ' | sed -e "s/$SYNC/\n$SYNC/g;" # 48 - 9 = 37

# Add a newline at the end of the output
echo 


