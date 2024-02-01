#!/bin/bash

set -euo pipefail

cdir="$(dirname "$(readlink -f "${0}")")"

source "${cdir}"/config.sh

# assume stereo
volbar_final_height=$(( vol_height * 2  ))

vid_final_height=$(( vid_height + volbar_final_height ))

function msg {
    echo "${@}" >&2
}

function make_volume {
    echo "[${1}]showvolume=m=p:w=${vid_width}:h=${vol_height}:dm=1:f=0:c=if(gte(VOLUME\,-0.2)\,0x0000ff\,if(gte(VOLUME\,-2)\,0x00ffff\,0x00ff00))[${2}_uncropped]; "
    echo "[${2}_uncropped] scale=${vid_width}:${volbar_final_height} [${2}]; "
}

function downscale {
    echo "[${1}] scale=${vid_width}:${vid_height} [${2}];"
}

function make_video_with_volume {
    # arg 1: name of input video stream
    # arg 2: name of input audio stream
    # arg 3: name of output stream
    make_volume "${2}" "vol_${1}"
    downscale "${1}" "${1}_scaled"
    echo "[${1}_scaled] [vol_${1}] vstack [${3}] ;"
}

function draw_black_box {
    echo "drawbox=c=black:w=${1}:h=${2} [${3}] ;"
}

function stack_streams_into_grid {
    # arg1: output stream name
    # rest of the args: alternating (paired) video and audio input stream names
    local out_stream="${1}"
    shift

    local n=0
    local strm
    local all_specs=""
    local xspec="0"
    local yspec="0"
    for i in $(seq 0 $(( grid_x - 1 )) ); do
        for j in $(seq 0 $(( grid_y - 1 )) ); do
            strm="vbox_${n}"
            if [[ $# -ge 1 ]]; then
                make_video_with_volume "${1}" "${2}" "${strm}"
                all_specs="${all_specs}|${xspec}_${yspec}"
                yspec="${yspec}+h${j}"
            else
                # govnokod, sorry
                break 2
            fi
            shift 2
            n=$(( n + 1 ))
        done
        xspec="${xspec}+w${i}"
        yspec="0"
    done
    all_specs="$(echo "${all_specs}" | cut -c2-)"

    for k in $( seq 0 $(( n - 1 )) ); do
        echo -n "[vbox_${k}]"
    done

    echo "xstack=inputs=${n}:layout=${all_specs}[${out_stream}]"
}

function do_stream {
    in_args=()
    streams=()
    i=0
    for arg in "${@}"; do
        streams+=("${i}:v" "${i}:a")
        in_args+=("-i")
        in_args+=("${arg}")
        i=$(( i + 1 ))
    done

    filters="$(stack_streams_into_grid vo "${streams[@]}")"
    msg "filters: ${filters}" >&2

    # TODO: fix ffmpeg args to actually do streaming
    ffmpeg -y -r 24 "${in_args[@]}" \
        -filter_complex "${filters}" \
        -map '[vo]' -r "${restream_fps}" -c:v libx264 -crf 19 \
        -f "${restream_format}" \
        "${restream_target}"
}

function do_play {
    in_args=()
    streams=()
    i=0
    for arg in "${@}"; do
        if [[ "${i}" -eq 0 ]]; then
            in_args+=("${arg}")
        else
            in_args+=("--external-file=${arg}")
        fi
        streams+=("vid$(( i + 1 ))" "aid$(( i + 1 ))")
        i=$(( i + 1 ))
    done

    filters="$(stack_streams_into_grid vo "${streams[@]}")"
    msg "filters: ${filters}" >&2

    mpv "${in_args[@]}" --lavfi-complex="${filters}" --alpha=no --no-resume-playback --pause=no
}

if [[ $# -eq 0 ]]; then
    mode=""
else
    mode="${1}"
fi

case "${mode}" in
    play)
        do_play "${@:2}"
        ;;
    stream)
        do_stream "${@:2}"
        ;;
    *)
        msg "${0} displays or streams a number of video sources in a grid"
        msg "usage: ${0} (play|stream) video_input1 video_input2 ..."
        exit 1
        ;;
esac
