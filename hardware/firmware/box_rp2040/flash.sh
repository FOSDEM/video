#!/bin/bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"

echo "Building firmware..."
"${cdir}"/build.sh

echo "Flashing firmware..."
"$cdir"/../tools/flash_rp2040_firmware.sh "$cdir"/build/src/box_rp2040.elf
