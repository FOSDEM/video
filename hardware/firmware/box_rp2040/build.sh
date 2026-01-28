#!/bin/bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"
if [[ $# -ge 1 ]]; then
    build_dir="$(readlink -f "${1}")"
else
    build_dir="${cdir}"/build
fi

if [[ ! -f "${cdir}"/vendor/pico-sdk/pico_sdk_init.cmake ]]; then
    cd "${cdir}"
    git submodule update --init
    cd "${cdir}"/vendor/pico-sdk
    git submodule update --init
fi

if [[ ! -d "${build_dir}" ]]; then
    mkdir "${build_dir}"
    cd "${build_dir}"
    cmake "${cdir}"
fi

echo "building"
cd "${build_dir}"
export CMAKE_POLICY_VERSION_MINIMUM=3.5
make -j8
