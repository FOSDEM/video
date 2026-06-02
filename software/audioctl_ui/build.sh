#!/bin/bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"

cd "${cdir}"

if ! [[ package.json -ot node_modules/marker && package-lock.json -ot node_modules/marker ]]; then
    npm install && touch node_modules/marker
fi

if [[ $# -eq 0 || "${1}" == build ]]; then
    npm run build
elif [[ "${1}" == serve ]]; then
    npm run dev
fi
