#!/usr/bin/env bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"

function run_linters {
    while read goproj_dir; do
        (
            set -euo pipefail
            cd "${cdir}"
            cd "${goproj_dir}"
            go test ./... 2>&1 | grep -v "^\(ok\|?\)" || true >&2
            if [[ -f .golangci.yaml ]]; then
                golangci-lint run
                golangci-lint fmt
            fi
        )
    done < <(
        echo "${cdir}"
        cat "${cdir}"/go.mod | grep '^replace ' | sed -E 's/.*=>\s*(.*)/\1/g'
    )
}

function do_build {
    mkdir -p "${cdir}"/build
    (
        set -euo pipefail
        cd "${cdir}"
        go build -o build/audioctl 'github.com/fosdem/video/software/audioctl/cmd/audioctl'
    )
    run_linters
}

function reset_go_mod {
    cat > "${cdir}"/go.mod <<END
    module github.com/fosdem/video/software/audioctl

    go 1.24
END
    (
        set -euo pipefail
        cd "${cdir}"
        export GOPROXY=direct
        go mod tidy
    )
}

function usage {
    cat <<END
usage:
    - ${1} build
    - ${1} run <args>
    - ${1} update-deps
END
    exit 1
}

if [[ $# -eq 0 ]]; then
    usage "${0}"
fi

case "${1}" in
    build)
        do_build
        ;;
    run)
        do_build
        "${cdir}"/build/audioctl "${@:2}"
        ;;
    update-deps)
        reset_go_mod
        do_build
        ;;
    *) usage "${0}" ;;
esac
