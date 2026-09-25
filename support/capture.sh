#!/bin/bash
#
# Capture every packet the flight software container sees, including loopback.
#
# WHY A SIDECAR AND NOT A PUBLISHED PORT
#
# Publishing a port (-p) exposes a service outward; it does nothing for packet
# capture. What capture needs is access to the container's network namespace.
#
# This matters here because NOS3 traffic splits in two:
#
#   - Bridge traffic (FSW <-> nos-engine-server, truth42sim, cosmos, ...) rides
#     the nos3-<sc> Docker network and is visible on the host's br-* interface.
#   - Loopback traffic (SpaceCOP <-> SCML on ports 9111 and 9112) never leaves
#     the FSW container's lo interface. It is invisible from the host and from
#     every other container.
#
# Running tshark in a container started with --net=container:<fsw> puts the
# capture inside the FSW container's namespace, so "-i any" sees both. Nothing
# about the flight software container or its launch needs to change.
#
# Usage:
#   ./support/capture.sh [container] [output.pcapng]
#
#   container     defaults to sc01-nos-fsw (SC_NUM is "sc0"$i in
#                 scripts/fsw/fsw_cfs_launch.sh, so spacecraft 1 is sc01)
#   output        defaults to capture/nos3-<timestamp>.pcapng under the repo
#
# Stop with Ctrl-C. Start it after `make launch`, once the FSW container exists.

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BASE_DIR=$( cd -- "$SCRIPT_DIR/.." &> /dev/null && pwd )

CONTAINER="${1:-sc01-nos-fsw}"
OUT_DIR="$BASE_DIR/capture"
OUT_FILE="${2:-$OUT_DIR/nos3-$(date +%Y%m%d-%H%M%S).pcapng}"
IMAGE="nos3-tshark:local"

if ! docker inspect "$CONTAINER" > /dev/null 2>&1; then
    echo ""
    echo "    Container '$CONTAINER' not found."
    echo "    Is NOS3 running? Try: docker ps --format '{{.Names}}'"
    echo ""
    exit 1
fi

echo "Ensuring capture image..."
docker build -q -t "$IMAGE" --build-arg BASE_IMAGE="${DBOX:-ivvitc/nos3-64:20260619}" \
    -f "$SCRIPT_DIR/Dockerfile.tshark" "$SCRIPT_DIR" > /dev/null

mkdir -p "$(dirname "$OUT_FILE")"

echo ""
echo "    Capturing traffic in the namespace of: $CONTAINER"
echo "    Writing: $OUT_FILE"
echo "    Ctrl-C to stop."
echo ""

# --net=container: shares the FSW container's namespace, which is what makes
#   loopback visible. It is mutually exclusive with --network and -p.
# NET_RAW/NET_ADMIN are needed to open a capture handle in promiscuous mode.
# The ring buffer keeps an orbit-length capture from filling the disk: 20 files
#   of ~100 MB, oldest discarded. Drop -b to capture to a single growing file.
docker run --rm -it \
    --net="container:$CONTAINER" \
    --cap-add=NET_RAW \
    --cap-add=NET_ADMIN \
    -v "$BASE_DIR:$BASE_DIR" \
    --name nos3-capture \
    "$IMAGE" \
    tshark -i any -w "$OUT_FILE" -b filesize:100000 -b files:20

echo ""
echo "    Capture written under $OUT_DIR"
echo "    Note: files are owned by root (capture requires it); chown if needed."
echo ""
