#!/bin/bash
# Runs cppcheck on CI

ci_app="../fsw"
io_lib="../../io_lib/fsw"
c3i_lib="../../c3i_lib/fsw"

output_file="./cppcheck_ci.txt"
error_file="./cppcheck_errors_ci.txt"

command -v cppcheck >/dev/null 2>&1 || { echo >&2 "Error: Requires cppcheck but it's not installed.  Aborting."; exit 1; }

paths_to_check="$ci_app/src/ $ci_app/examples/multi/ $ci_app/examples/multi_tf/ $ci_app/examples/rs422/ $ci_app/examples/udp/ $ci_app/examples/udp_dem/"

include_dirs="-I $ci_app/platform_inc/ -I $ci_app/mission_inc -I $io_lib/public_inc  -I $c3i_lib/public_inc"

flags="-v --report-progress --std=c89"

cppcheck $flags $include_dirs $paths_to_check 2> $error_file > $output_file
