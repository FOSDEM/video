#!/bin/sh

set -euo pipefail

tty=/dev/tty_fosdem_audio_ctl
build_file=teensy_audio.hex

cdir="$(dirname "$(readlink -f "${0}")")"
if [[ $# -ge 1 ]]; then
    build_dir="$(readlink -f "${1}")"
else
    build_dir="${cdir}"/build
fi

if [[ -e "${tty}" ]]; then
    stty -F "${tty}" 134
    sleep 0.5
fi

for i in {1..3}; do
    if teensy_loader_cli --mcu TEENSY41 -v -w "$build_dir/$build_file"; then
        exit 0
    fi
done
