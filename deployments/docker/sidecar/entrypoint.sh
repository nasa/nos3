#!/bin/bash -x

curl -X POST http://active-gs:8090/api/links/nos3/radio-in:disable
curl -X POST http://active-gs:8090/api/links/nos3/radio-out:disable
curl -X POST http://active-gs:8090/api/links/nos3/truth42-in:disable

sleep 11

curl -X POST http://active-gs:8090/api/links/nos3/radio-in:enable
curl -X POST http://active-gs:8090/api/links/nos3/radio-out:enable
curl -X POST http://active-gs:8090/api/links/nos3/truth42-in:enable

pip install --upgrade yamcs-client && python3 /commanding.py

tail -f /dev/null
