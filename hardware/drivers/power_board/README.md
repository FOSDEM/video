# Power board kernel driver

Debian building:

```shell-session
$ apt install build-essential linux-headers-`uname -r`
$ make
```

Loading:

```shell-session
$ insmod power_board.ko
```

Unloading:

```shell-session
$ rmmod power_board
```

## Usage

* The driver registers with the hwmon subsystem to show monitoring stats in lm-sensors
* sysfs nodes are created for reading the data from scripts at `/sys/class/usbmisc/powerboard0/device`
