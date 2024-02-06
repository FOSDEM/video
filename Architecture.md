# General overview of the video streaming/recording architecture for FOSDEM 

## TLDR

camera & presenter laptop -> videoboxes -> voctops -> restreamer -> caches <- browsers

For a drawing, see graph/fosdem-video-2017.pdf.

## Elements

The main elements are:

* Cameras and presenter laptops (sources for video)
* Microphones
* Videoboxes
* Voctops (laptops with voctomix)
* Dumpers & pushers
* Restreamers

## In-room setup

Better described in the instruction manual.

Here we have the microphones connected to an audio mixer (in the case of the smallest rooms, the mixer is skipped), which goes then in one of the inputs of the camera. The camera is connected to the camera videobox over HDMI, and the videobox records the input and streams it in MPEG-TS over TCP. The presenter's laptop is connected to the presenter videobox, which also records the input and streams it in the same way. 

Both video boxes display on their screens a screenshot of what they push out over TCP (The stream gets tee'd. Look at how sproxy gets used...).

For the hardware in the videoboxes, see `hardware/README.md`.

For the software running on the videoboxes and their configs, see `software/ansible/playbooks/roles/videobox/`.

## Video mixing

All the voctops are stored in a central location, and each one runs an instance of voctomix (https://github.com/voc/voctomix/).

Each voctomix is fed with three sources:

* camera
* presenter
* background

The camera and presenter sources are fed using ffmpeg from the multicast UDP stream, the background is also fed with ffmpeg, using a raw video image. 

The output of voctomix is encoded to H.264 and sent again as MPEG-TS over multicast UDP.

All the voctomix scripts can be seen in `software/ansible/playbooks/roles/voctop/files/scripts/`.

## Video streaming and recording

The voctop then forwards the H.264 UDP stream to the main restreamer at scaleway and the backup, using the `streamcast.sh` and `streamcast_backup.sh` scripts.

The recordings of the final material are done on the main restreamer and the backup. In addition, all UDP multicast streams (the single streams from every videobox as well as the merged stream per room) are also dumped on two separate laptops with the ansible role `stream-dumper`.

The main restreamer is nginx that receives streams via RTMP (https://github.com/arut/nginx-rtmp-module). It stores primary recordings and serves HLS to the caching servers.

The end-users connect to the caching servers and see the streams via HLS.
