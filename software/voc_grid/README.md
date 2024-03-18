## voc_grid

This script displays multiple streams in a grid, together
with audio levels, for real-time monitoring purposes.

Streams are scaled down to thumbnail-like.

### Usage

```bash
$ ./run.sh (stream|play) source1 source2 ...
```

The script has two possible modes: stream and play.
In stream mode, the grid is sent as a stream to the location configured in the
config file.
In play mode, the grid is shown on the local machine via mpv.

The script is configured via `config.sh`.

Each source can be a HLS stream, a RTMP stream or whatever ffmpeg understands.

You can also append `@<video stream number>:<audio stream number>` to each source
to select a specific video/audio stream from it (if it contains multiple)

Here's an example:

```
$ ./run.sh play 'tcp://voctop24.video.fosdem.org:8899@2:1' 'tcp://voctop25.video.fosdem.org:8899@2:1' 'tcp://voctop26.video.fosdem.org:8899@2:1' 'tcp://voctop27.video.fosdem.org:8899@2:1'
$ # '@2:1' means to select the second video stream (which is 720p) and the first audio stream
```

### TODO

Hardware acceleration is not fully utilised at the moment: all the compositing
can be done via vaapi. This needs to be implemented further.
