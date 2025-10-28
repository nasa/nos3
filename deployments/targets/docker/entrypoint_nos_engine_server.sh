#!/bin/bash

pkill -f nos_engine_server_standalone || true

/usr/bin/nos_engine_server_standalone /builds/nos3/sims/build/bin/nos_engine_server_config.json

tail -f /dev/null
