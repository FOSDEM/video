#!/bin/bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"

if [[ $# -eq 1 ]]; then
    dirs=( "$(readlink -f "${1}")" )
elif [[ $# -eq 0 ]]; then
    dirs=( "${cdir}"/src )
else
    echo 'what?' >&2
    exit 1
fi

cd "${cdir}"

for dir in "${dirs[@]}"; do
    echo "formatting in ${dir}" >&2
    find "${dir}" -type f -regextype egrep -iregex '.*\.[ch]|.*\.cpp' \
        -not -path '*/sys/*' \
        -not -path '*/build/*' \
        -not -path '*/vendor/*' \
        -exec clang-format -i -style=file '{}' \+
done
