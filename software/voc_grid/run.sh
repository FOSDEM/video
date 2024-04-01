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

function die {
    msg "${@}"
    exit 1
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

function get_num_streams_and_filename {
    local filename="$(echo "${1}" | cut -d '@' -f 1)"
    local req_video_stream_number="$(echo "${1}" | cut -s -d '@' -f 2 | cut -s -d ':' -f 1)"
    local req_audio_stream_number="$(echo "${1}" | cut -s -d '@' -f 2 | cut -s -d ':' -f 2)"
    if [[ -z "${req_video_stream_number}" ]]; then
        req_video_stream_number=1
    fi
    if [[ -z "${req_audio_stream_number}" ]]; then
        req_audio_stream_number=1
    fi

    local stream_data="$(ffprobe "${filename}" 2>&1 | grep -P 'Stream #.*(Video|Audio)')"
    local num_video_streams="$(echo "${stream_data}" | grep -F "Video:" | wc -l)"
    local num_audio_streams="$(echo "${stream_data}" | grep -F "Audio:" | wc -l)"
    if [[ ${num_video_streams} -le 1 ]]; then
        num_video_streams=1
    fi
    if [[ ${num_audio_streams} -le 1 ]]; then
        num_audio_streams=1
    fi
    msg "${filename} has ${num_video_streams} video streams and ${num_audio_streams} audio streams"
    msg "filename: ${filename}; requested video stream: ${req_video_stream_number}; requested audio stream: ${req_audio_stream_number}"

    if ! [[ "${req_video_stream_number}" -ge 1 && "${req_video_stream_number}" -le "${num_video_streams}" ]]; then
        die "requested video stream number ${req_video_stream_number} but there are ${num_video_streams} video streams"
    fi
    if ! [[ "${req_audio_stream_number}" -ge 1 && "${req_audio_stream_number}" -le "${num_audio_streams}" ]]; then
        die "requested audio stream number ${req_audio_stream_number} but there are ${num_audio_streams} audio streams"
    fi
    echo "${num_video_streams} ${num_audio_streams} ${req_video_stream_number} ${req_audio_stream_number} ${filename}"
}

function do_stream {
    local in_args=()
    local streams=()
    local i_audio=0
    local i_video=0
    for arg in "${@}"; do
        read num_video_streams num_audio_streams req_video_stream_number req_audio_stream_number filename < <(get_num_streams_and_filename "${arg}")

        streams+=("$(( i_video + req_video_stream_number - 1)):v" "$(( i_audio + req_audio_stream_number - 1 )):a")
        in_args+=("-i")
        in_args+=("${filename}")

        # take the first stream from each given source
        # TODO: implement way to select non-first stream
        i_audio=$(( i_audio + num_audio_streams ))
        i_video=$(( i_video + num_video_streams ))
    done

    filters="$(stack_streams_into_grid vo "${streams[@]}")"
    msg "filters: ${filters}" >&2

    # TODO: fix ffmpeg args to actually do streaming
    ffmpeg -y -r 24 "${in_args[@]}" \
        -filter_complex "${filters}" \
        -map '[vo]' -r "${restream_fps}" -c:v libx264 -crf 19 \
        i=$(( i + 1 ))
        -f "${restream_format}" \
        "${restream_target}"
}

function do_play {
    local in_args=()
    local streams=()
    local i_audio=0
    local i_video=0
    for arg in "${@}"; do
        read num_video_streams num_audio_streams req_video_stream_number req_audio_stream_number filename < <(get_num_streams_and_filename "${arg}")

        if [[ "${i_audio}" -eq 0 ]]; then
            # first source must be given as argument
            in_args+=("${filename}")
        else
            in_args+=("--external-file=${filename}")
        fi
        streams+=("vid$(( i_video + req_video_stream_number ))" "aid$(( i_audio + req_audio_stream_number ))")

        # take the first stream from each given source
        # TODO: implement way to select non-first stream
        i_audio=$(( i_audio + num_audio_streams ))
        i_video=$(( i_video + num_video_streams ))
    done

    filters="$(stack_streams_into_grid vo "${streams[@]}")"
    msg "filters: ${filters}" >&2
    msg "files: ${in_args[@]}"

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
