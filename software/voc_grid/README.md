## voc_grid

This script displays multiple streams in a grid, together
with audio levels, for real-time monitoring purposes.

Streams are scaled down to thumbnail-like.

### Usage

```bash
$ ./run.sh (stream|play) stream1 stream2 ...
```

The script has two possible modes: stream and play.
In stream mode, the grid is sent as a stream to the location configured in the
config file.
In play mode, the grid is shown on the local machine via mpv.

The script is configured via `config.sh`.

### TODO

Hardware acceleration is not fully utilised at the moment: all the compositing
can be done via vaapi. This needs to be implemented further.
