#!/usr/bin/env python3
import serial
import os.path
import time
import argparse
import subprocess

# Dumps out the crash report data from the debug port of the teensy
# and runs it through addr2line. Also deals with the port disappearing
# and reappearing and making sure the teensy boots after crashing

parser = argparse.ArgumentParser(description="Audio crashreport decoder")
parser.add_argument("elf")
args = parser.parse_args()

port = "/dev/tty_fosdem_audio_debug"

while True:
    if not os.path.exists(port):
        time.sleep(0.3)
        continue

    try:
        ser = serial.Serial(port)
        print("Debug port available...")
        while True:
            line = ser.readline().decode()
            if "Check code" in line:
                addr = line.split()[3]
                print('Crash at', addr)
                subprocess.run(['arm-none-eabi-addr2line','-f', '-e', args.elf, addr.strip()])
            elif 'mysketch' in line:
                continue
            else:
                print("::", line.strip())
    except Exception as e:
        time.sleep(1)
