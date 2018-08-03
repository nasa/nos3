#!/usr/bin/env python
#

import zmq
#
# Receive zeroMQ messages (used for debugging only - not used by CFS Ground System)
#
def main():
    """ main method """

    # Prepare our context and publisher
    context    = zmq.Context()
    subscriber = context.socket(zmq.SUB)
    subscriber.connect("ipc:///tmp/GroundSystem")
    subscriber.setsockopt(zmq.SUBSCRIBE, b"GroundSystem")

    while True:
        # Read envelope with address
        [address, contents] = subscriber.recv_multipart()
        print("[%s] %s" % (address, contents))

    # We never get here but clean up anyhow
    subscriber.close()
    context.term()


if __name__ == "__main__":
    main()
