#!/bin/bash

readonly json_file_path="${1}"
readonly json_file=$(cat "${json_file_path}")

function get_room_info {
    local -r room_info_json=$(printf '%s\n' "${json_file}" | jq --arg room "${room_name}" '.[] | select(.Room == $room) | {camera_position, best_cable_run}' )
    printf '%s\n' "${room_info_json}"
    return 0
}

function populate_room_checklist {
    local -r room_info_json="${1}"
    local -r camera_position=$(printf '%s\n' "${room_info_json}" | jq -r '.camera_position')
    local -r best_cable_run=$(printf '%s\n' "${room_info_json}" | jq -r '.best_cable_run')
    sed -i "s|ROOM_NUMBER|${room_name}|g" "${path_to_room_checklist}"
    sed -i "s|CAMERA_POSITION|\"${camera_position}\"|g" "${path_to_room_checklist}"
    sed -i "s|CABLE_RUN|${best_cable_run}|g" "${path_to_room_checklist}"
    return 0
}

function create_and_populate_room_checklist_file {
    local -r room_name="${1}"
    local -r path_to_room_checklist="per-room-checklists/${room_name}-setup-checklist.md"
    cp 'FOSDEM_Room_setup_checklist_template.md' "${path_to_room_checklist}"
# shellcheck disable=SC2046
    local -r info_json=$(get_room_info)
    populate_room_checklist "${info_json}"


}

readonly array_of_rooms=$(printf '%s\n' "${json_file}" | jq -r '.[].Room')

for devroom in $array_of_rooms
do
    create_and_populate_room_checklist_file "${devroom}" &
done

wait

exit 0
