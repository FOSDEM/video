#!/bin/sh


VOCTOP="$(curl "https://control.video.fosdem.org/query-room.php?room=$1" | grep -ohE 'voctop[0-9]+')"
echo $VOCTOP
CAM=$(curl "https://control.video.fosdem.org/query-vocto.php?voctop=$VOCTOP.video.fosdem.org" | cut -d' ' -f 3)
echo $CAM

mpv --lavfi-complex='[aid1] asplit [t1] [ao] ; [t1] showvolume=w=700:h=100 [t2] ; [vid1]  [t2]  overlay  [vo]' tcp://$CAM:8899
