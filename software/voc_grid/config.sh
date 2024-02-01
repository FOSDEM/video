#!/bin/bash

vid_width=320               # width of each video box
vid_height=180              # height of each video box
vol_height=40               # height of vu meter under each video box
vol_default_num_channels=2  # if video has a different number of channels than specified, it will work fine but will be uglier
grid_x=4                    # number of horizontal subdivisions
grid_y=2                    # number of vertical subdivisions
# the total number of video boxes is equal to grid_x * grid_y

# restream options (only used in re-streaming mode)

restream_fps=24
restream_target=rtmp://strm.ludost.net/st/ivantestva
restream_format=flv
