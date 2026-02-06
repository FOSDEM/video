#! /bin/bash
while true; do
	xrandr --auto --output HDMI-2 --mode 1920x1080 && sleep 5s
#	xrandr --auto --output HDMI-2 --mode 4096x2160 && sleep 2s
#	xrandr --auto --output HDMI-2 --mode 3840x2160 && sleep 2s
#	xrandr --auto --output HDMI-2 --mode 2560x1440 && sleep 2s
	xrandr --auto --output HDMI-2 --mode 1280x720 && sleep 5s
	xrandr --auto --output HDMI-2 --mode 1024x768 && sleep 5s
	xrandr --auto --output HDMI-2 --mode 800x600 && sleep 5s
	xrandr --auto --output HDMI-2 --mode  640x480 && sleep 5s
done
