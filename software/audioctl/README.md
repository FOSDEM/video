## audioctl

This application is used for controlling the rev-E and later
[FOSDEM audio boards](../../hardware/pcbs/audio_board). The
[firmware](../../hardware/firmware/audio_board) of these
boards uses a custom simple text-based protocol to communicate
over serial over USB.

For earlier revision boards, which are controlled using hybrid
dual USB serial endpoints with OSC over SLIP over serial over USB,
see [video-amixcontrol](https://github.com/fosdem/video-amixcontrol).

### Running

```
$ cp example_config.yaml my_config.yaml
<modify my_config.yaml>
$ ./build.sh run my_config.yaml
```

For debug purposes, you can set `port_device: fake` in the config
to start the server without actual hardware.
