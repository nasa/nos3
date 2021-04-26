#!/bin/bash
#
# Convenience script for NOS3 development
#

# cFS
killall -q -r -9 core-cpu*

# COSMOS
killall -q -9 ruby

# NOS3
killall -q -INT -r 'nos3.*simulator.*'
killall -q nos_engine_server_standalone
killall -q nos-time-driver
killall -q 42
