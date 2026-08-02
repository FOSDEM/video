#!/usr/bin/env python3

import json
import os
import re
import signal
import subprocess
import sys
import syslog
import time
import tomllib

import serial

WHITE = 255, 255, 255
BLACK = 0, 0, 0
GREEN = 0, 255, 0
RED = 255, 0, 0

NORMAL = 0
GOOD = 1
BAD = 2
CHECK = 3

os.environ["LANG"] = "C"

LOGO_FILE = '/usr/local/bin/logo.png'

portnames = ["IN", "01", "02", "03", "04"]


class stateEntry():
    """ entry in the states """
    data: str
    nl: bool
    state: int  # 0 NORMAL 1 GOOD(green) 2 BAD(red) 3 CHECK(yellow)

    def __init__(self, inpdata, inpstate=0, nl=True):
        self.data = inpdata
        self.state = inpstate
        self.nl = nl


def port_name_to_idx(name):
    for i, n in enumerate(portnames):
        if n == name:
            return i


def configure_switch():
    with get_serial() as port:
        if not os.path.isfile("/etc/network/switch.toml"):
            syslog.syslog("No switch config file found, disabling vlan support")
            port.write(b"netswitch.vlan-init 0\n")
            return
    with open("/etc/network/switch.toml", "rb") as handle:
        config = tomllib.load(handle)

    with get_serial() as port:
        port.write(b"netswitch.vlan-init 0\n")

        if not config['vlans']:
            syslog.syslog("VLAN support is disabled in the config")
            return

        port.write(b"netswitch.vlan-init 1\n")

        memberconfig = 1
        all_tagged = set()
        all_untagged = set()
        for vlan_name in config:
            if not isinstance(config[vlan_name], dict):
                continue

            vlan = config[vlan_name]

            vid = vlan['vlan']
            members = 0
            tagged = 0
            untagged = 0
            syslog.syslog(f"Creating vlan '{vlan_name}' with vid {vid}")

            for p in vlan['tagged']:
                members |= 1 << port_name_to_idx(p)
                tagged |= 1 << port_name_to_idx(p)
                all_tagged.add(p)
            for p in vlan['untagged']:
                members |= 1 << port_name_to_idx(p)
                untagged |= 1 << port_name_to_idx(p)
                all_untagged.add(p)
            print(f"VLAN {vid} tagged {tagged} untagged {untagged} members {members}")

            port.write(f"netswitch.vlan-entry-define {vid} {members} {untagged}\n".encode())

            if untagged != 0:
                port.write(f"netswitch.vlan-member-define {memberconfig} {vid} {untagged}\n".encode())
                print(f"netswitch.vlan-member-define {memberconfig} {vid} {untagged}\n".encode())
                memberconfig += 1

        for p in all_tagged - all_untagged:
            port.write(f"netswitch.vlan-filtering {port_name_to_idx(p)} tagged-only\n".encode())
        for p in all_untagged - all_tagged:
            port.write(f"netswitch.vlan-filtering {port_name_to_idx(p)} untagged-only\n".encode())
        for p in all_tagged & all_untagged:
            port.write(f"netswitch.vlan-filtering {port_name_to_idx(p)} all\n".encode())


def render_text(states):
    def strRed(skk):
        return "\033[91m" + skk + "\033[00m"

    def strGreen(skk):
        return "\033[92m" + skk + "\033[00m"

    def strYellow(skk):
        return "\033[93m" + skk + "\033[00m"

    out = ""

    maxLen = 0

    for state in states:
        maxLen = max(maxLen, len(state.data))

    maxLen = int(maxLen / 8 + 1) * 8
    cline = "| "
    for state in states:
        if state.state == GOOD:
            rend = strGreen(state.data)
        elif state.state == BAD:
            rend = strRed(state.data)
        elif state.state == CHECK:
            rend = strYellow(state.data)
        else:
            rend = state.data

        cline += rend
        if state.nl:
            out += cline + " " * (maxLen - len(cline)) + " |\n"
            cline = "| "

    return out


def render_commands(states):
    def strRed(skk):
        return b"\x1b\x01" + skk.encode("utf8")

    def strGreen(skk):
        return b"\x1b\x0e" + skk.encode("utf8")

    def strYellow(skk):
        return b"\x1b\x03" + skk.encode("utf8")

    def strWhite(skk):
        return b"\x1b\x0f" + skk.encode("utf8")

    out = b"\n\n"
    out += b"display.img.clear\n"

    line = 0

    cline = b""
    for state in states:
        if state.state == GOOD:
            rend = strGreen(state.data)
        elif state.state == BAD:
            rend = strRed(state.data)
        elif state.state == CHECK:
            rend = strYellow(state.data)
        else:
            rend = strWhite(state.data)
        cline += rend
        if state.nl:
            out += b"display.text.line " + str(line).encode("utf8") + b" " + cline + b"                 \n"
            line += 1
            cline = b""

    out += b"display.refresh\n"
    # print(out)
    return out


def get_serial():
    while True:
        try:
            s = serial.Serial('/dev/tty_fosdem_box_ctl', 115200, timeout=1, exclusive=True)
        except:
            continue
        break

    return s


def output_terminal(states):
    sys.stdout.write("\x1b7\x1b[%d;%df%s\x1b8" % (0, 0, render_text(states)))
    sys.stdout.flush()


def clear_serial_display():
    with get_serial() as port:
        port.write(b"display.text.clear")


def output_serial_display(states):
    cmds = render_commands(states)
    with get_serial() as port:
        port.write(cmds)


img_x = 240
img_y = 134
xpos = int((img_x - img_y) / 2)


def output_image():
    try:
        f = open("/tmp/picture.raw", "rb");
        data = f.read()
        f.close()
        l = len(data)
        expected = img_x * img_y * 2
        if l != expected:
            syslog(f"/tmp/picture.raw has size {l} bytes expected {expected} bytes")
            return
    except:
        return

    with get_serial() as port:
        port.write(b"display.text.clear\n")
        port.write(b"display.img.clear\n")

        dcmd = f"display.img 565 0 {xpos} {img_x} {img_y}\n"
        port.write(dcmd.encode("utf-8"))
        port.write(data)
        port.write(b"\ndisplay.imgonly\n")


switch_state = [None, None, None, None, None]


def start_chargers():
    with get_serial() as port:
        port.write(b"pb.chargers.on 1\n")


def read_switch():
    ret = [None, None, None, None, None]

    updated = False
    with get_serial() as port:
        port.write(b"netswitch.info\n")
        while True:
            line = port.readline().decode("utf-8").strip()
            m = re.match(r"port ([0-9]): (.*)$", line)
            if m is not None:
                p = int(m.group(1))
                s = m.group(2)
                ret[p] = s
                updated = True
            elif re.match(r"^(ok|fail) ", line):
                break

    if not updated:
        return None
    else:
        try:
            with open("/tmp/netstate.tmp", "w") as f:
                f.write(json.dumps(switch_state))

            os.rename("/tmp/netstate.tmp", "/tmp/netstate.json")
        except:
            pass

        return ret


def update_switch_state():
    new_state = read_switch()

    if new_state is None:
        return

    for i in range(0, 5):
        if new_state[i] != switch_state[i]:
            syslog.syslog(f"Port {i} state change {switch_state[i]} -> {new_state[i]}")
            switch_state[i] = new_state[i]


diag = ''


def update_diag():
    global diag
    newdiag = ''
    with get_serial() as port:
        port.write(b"status\n")
        while True:
            line = port.readline().decode("utf-8").strip()
            if re.match(r"^(ok|fail) ", line):
                break
            newdiag += "\n" + line

    if diag != newdiag:
        diag = newdiag
    else:
        try:
            with open("/tmp/status.tmp", "w") as f:
                f.write(diag)

            os.rename("/tmp/status.tmp", "/tmp/status.txt")
        except:
            pass


def main():
    signal.signal(signal.SIGHUP, signal_handler)

    counter = 0
    configure_switch()

    os.system("clear")
    syslog.openlog("video-status")
    update_switch_state()
    start_chargers()

    while True:
        info = update_sysinfo()
        update_switch_state()
        update_diag()
        output_terminal(info)
        if counter % 3 == 0:
            output_serial_display(info)
        else:
            output_image()
        counter = (counter + 1) % 256
        time.sleep(1)


def update_sysinfo():
    ret = []
    # Hostname
    hostname = os.popen('hostname -s').read().strip()

    # Uptime
    uptime = os.popen('uptime').read().strip()
    matches = re.search(
        '\s?(.*)\s+up\s+(.*?),\s+([0-9]+) users?,\s+load average: ([0-9]+\.[0-9][0-9]),?\s+([0-9]+\.[0-9][0-9]),?\s+([0-9]+\.[0-9][0-9])',
        uptime)
    uptime_time, uptime_duration, uptime_users, uptime_avg1, uptime_avg5, uptime_avg15 = matches.groups()

    # Interface

    try:
        ifdata = json.loads(
            subprocess.check_output("ip -j route get 8.8.8.8", shell=True, stderr=subprocess.DEVNULL).decode("utf-8"))
        interface = ifdata[0]["dev"]
        # IP addresses
        addr_data = json.loads(
            subprocess.check_output(f"ip -j addr show dev {interface} primary scope global", shell=True,
                                    stderr=subprocess.DEVNULL).decode("utf-8"))
        ip_link_mac = addr_data[0]["address"]

        ip_prefix_v4 = False
        ip_addr_v4 = False

        for a in addr_data[0]["addr_info"]:
            try:
                if a["family"] == "inet":
                    ip_prefix_v4 = str(a["local"]) + "/" + str(a["prefixlen"])
                    ip_addr_v4 = str(a["local"])
            except KeyError:
                pass

    except:
        interface = None
        ip_prefix_v4 = False
        ip_addr_v4 = False
        ip_link_mac = "UNKNOWN"

    # Signal
    # width: 1920
    # height: 1200
    # signal: no

    try:
        signaldata = json.loads(open('/tmp/ms213x-status', 'r').read())
        if signaldata['signal'] == 'yes':
            signal = True
            resX = signaldata['width']
            resY = signaldata['height']
            resolution = f"{resX}x{resY}"
        else:
            signal = False
    except Exception as e:
        #		print("exception")
        #		print(e)
        signal = False
    # print(signaldata)
    if not os.path.isfile("/tmp/ms213x-status"):
        ret.append(stateEntry("NO CAPTURE DEVICE", BAD))
    elif signal:
        ret.append(stateEntry(f"SIGNAL {resolution}"))
    else:
        ret.append(stateEntry("NO SIGNAL", BAD))

    ret.append(stateEntry(f"host: {hostname} ", nl=False))

    rec_info = os.popen('systemctl show video-recorder --property=ActiveState').read()
    if re.search('^ActiveState=active', rec_info) == None:
        ret.append(stateEntry(f"rec: paused ", BAD))
    else:
        ret.append(stateEntry(f"rec: RECORD ", GOOD))

    try:
        swstate = json.loads(open('/tmp/netstate.json', 'r').read())
        n = 0
        ret.append(stateEntry("Switch:", NORMAL, nl=False))
        for p in swstate:
            pn = portnames[n]
            if p == "down":
                ret.append(stateEntry(f"{pn}", BAD, nl=False))
            elif p == "up full-duplex 1000mbps":
                ret.append(stateEntry(f"{pn}", GOOD, nl=False))
            else:
                ret.append(stateEntry(f"{pn}", CHECK, nl=False))
            ret.append(stateEntry("|", NORMAL, nl=False))
            n += 1
        ret.append(stateEntry(""))
    except:
        ret.append(stateEntry("Switch not found", BAD))

    connected = subprocess.check_output("ss -H -o state established '( sport = :8899 )'  not dst '[::1]'|wc -l",
                                        shell=True).strip().decode("utf-8")

    ret.append(stateEntry(f"connected readers: {connected}", GOOD if int(connected) > 0 else NORMAL))

    sensordata = json.loads(subprocess.check_output("sensors -j 2>/dev/null", shell=True).decode("utf-8"))

    # root@box1:/usr/local/bin# sensors -j 2>/dev/null | jq '."thinkpad-isa-0000".temp1.t:semp1_input' |less
    # root@box1:/usr/local/bin# sensors -j 2>/dev/null | jq '."coretemp-isa-0000"."Package id 0"."temp1_input"'
    state = NORMAL
    cpu_temp = sensordata["coretemp-isa-0000"]["Package id 0"]["temp1_input"]
    if os.path.exists("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq"):
        fp = open('/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq', "r")
        hz = int(int(fp.read().rstrip()) / 1000)
        fp.close()
        if hz > 1800:
            state = NORMAL
        else:
            state = BAD
    else:
        if cpu_temp > 80:
            state = BAD
        else:
            state = NORMAL

    ret.append(stateEntry(f"cpu t:{cpu_temp}|f:{hz} MHz", state))

    ret.append(
        stateEntry(f"load: {uptime_avg1} {uptime_avg5} {uptime_avg15}", NORMAL if float(uptime_avg1) < 3.9 else BAD))

    if ip_addr_v4 != False:
        ret.append(stateEntry(f"IPv4: {ip_prefix_v4}"))
        ret.append(stateEntry(f"MAC: {ip_link_mac}"))
    else:
        ret.append(stateEntry(f"IPv4: no IPv4 address", BAD))
        ret.append(stateEntry(f"MAC: {ip_link_mac}"))

    if os.path.exists('/etc/fosdem_revision'):
        fp = open('/etc/fosdem_revision', "r")
        ret.append(stateEntry(fp.read().rstrip()))
        fp.close()
    else:
        ret.append(stateEntry("revision not found"), BAD)

    return ret


def signal_handler(signum, frame):
    # We need something to catch signals since systemd sends a SIGHUP, if we
    # don't catch that, we'll quit and die
    pass


if __name__ == "__main__":
    main()
