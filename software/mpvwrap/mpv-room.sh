#!/bin/sh

mpv --lavfi-complex='[aid1] asplit [t1] [ao] ; [t1] showvolume=w=700:h=100 [t2] ; [vid1]  [t2]  overlay  [vo]'  $(curl "https://control.video.fosdem.org/query-room.php?room=$1")
