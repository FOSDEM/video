#!/bin/bash

set -euo pipefail

if [[ $# -ne 1 ]]; then
    echo "usage: ${0} <SSH host of radxa>"
    exit
fi

cdir="$(dirname "$(readlink -f "${0}")")"

"${cdir}"/build.sh

echo "sending firmware to ${1}"
scp "${cdir}"/build/teensy_audio.{hex,elf} "${1}":/tmp/
scp "${cdir}"/../tools/flash_audio_board_firmware.sh "${1}":/tmp
ssh "${1}" '/tmp/flash_audio_board_firmware.sh /tmp/teensy_audio.hex'
