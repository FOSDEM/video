#!/usr/bin/env bash

set -euo pipefail

tty=/dev/tty_fosdem_audio_ctl

if [[ $# -ne 1 ]] || [[ ! -f "${1}" ]]; then
    echo "usage: ${0} <firmware hex file>" >&2
    exit 1
fi

if which teensy_loader_cli &>/dev/null; then
    loader="$(which teensy_loader_cli)"
elif which teensy-loader-cli &>/dev/null; then
    loader="$(which teensy-loader-cli)"
else
    echo "teensy-loader-cli not found" >&2
    exit 1
fi

if ! { "${loader}" --list-mcus || true; } | grep -qE '\bTEENSY41\b'; then
    echo "${loader} on ${hostname} doesn't seem to support TEENSY41, quitting" >&2
    exit 1
fi

if [[ -e "${tty}" ]]; then
    echo 'firmware is currently running: telling it to restart to bootloader'
    stty -F "${tty}" 134
fi

i=0
while ! lsusb | grep -qE '\<16c0:0478\>'; do
    sleep 0.5
    i=$(( i + 1 ))
    if [[ "${i}" -gt 20 ]]; then
        echo 'teensy bootloader did not appear'
        exit 1
    fi
done

echo 'flashing'
for i in {1..3}; do
    if "${loader}" --mcu TEENSY41 -v -w "${1}"; then
        echo 'flashed successfully'
        exit 0
    else
        echo "flashing attempt ${i} failed"
    fi
done
echo 'giving up'
exit 1
