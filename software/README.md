# Software Architecture

The software hierarchy is dictated by 2 main goals: _recording_ and _streaming_
of the video streams encoded inside the box.

## Video boxes

The video boxes are the boxes we deploy to each room. These will run ffmpeg to
grab the output from the H.264 encoders and send that to our voctocore servers.

We are currently using a vanilla Debian GNU/Linux 11 as our base system.
Additional software is distributed and installed via Ansible.

## Faking a stream from a videobox

If you want to realisticly fake the streaming output of a video box, that is
easy! Use a recent version of `youtube-dl`:

    youtube-dl -f 22 https://www.youtube.com/watch?v=DLzxrzFCyOs -o 720p.mp4
    ffmpeg -re -i 720p.mp4 -c copy -f mpegts udp://227.0.0.2:9000

Testing your stream is easy. Use either mpv:

    apt-get install mpv
    mpv tcp://aw1126-slides.local:8898/

...or ffplay (from the ffmpeg package)

    ffplay tcp://aw1126-slides.local:8898/

You can also use the IPs instead of the .local addresses.

## Waking up all voctops

    cat ./hosts/voctops | cut -f3 | xargs -n1 wol

## Re-installing a videobox

The videoboxes will do PXE boot if they can not load an OS from the SD card.

To re-install a videobox, simply make sure it will not boot properly by nuking
the install on the SD card.

    dd if=/dev/zero of=/dev/mmcblk0 bs=512 count=1

## Streaming

## Addressing plan

Our dear networking team has assigned us 185.175.218.0/24. We will be using
static DHCP bindings for the boxes and vocto containers.

We will not use separate networks, but it never hurts to have some logic in your
addressing plan.

* Servers, control hosts etc. are in 185.175.218.0/26
* Vocto laptops are in 185.175.218.64/26
* Videoboxes are in 185.175.218.128/26

### Generating DHCP bindings and DNS records

    cd hosts
    cat voctops boxes | ./gen-a-records.sh > a-records
    cat voctops boxes | ./gen-ptr-records.sh > ptr-records
    ./dhcpdgen.sh < voctops > voctops.conf
    ./dhcpdgen.sh < boxes > boxes.conf

### Addresses

#### Control hosts

| MAC | hostname | IP |
|-----|----------|----|
| `d0:67:26:cc:86:56` | streamdump0 | 185.175.218.10 |
| `70:85:c2:a2:d2:16` | control | 185.175.218.11 |
| `00:26:99:c4:a3:42` | voctop-sw | 185.175.218.12 |

#### Voctops and video boxes

Listed in [hosts/voctops](hosts/voctops) and [hosts/boxes](hosts/boxes)

#### Staff laptops

| MAC | hostname | IP | Multicast | Remarks |
|-----|----------|----|-----------|---------|
