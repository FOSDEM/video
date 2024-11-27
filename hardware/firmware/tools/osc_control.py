#!/usr/bin/env python3
import argparse
import tabulate

from pythonosc.osc_message_builder import OscMessageBuilder

from typing import Union

from pythonosc.osc_bundle import OscBundle
from pythonosc.osc_message import OscMessage

import serial


class SLIPClient:
    END = b'\xc0'
    ESC = b'\xdb'
    ESC_END = b'\xdc'
    ESC_ESC = b'\xdd'

    def __init__(self, device, baudrate=9600, **kwargs):
        self.ser = serial.Serial(device, baudrate=baudrate, **kwargs)

    def send(self, content: Union[OscMessage, OscBundle]) -> None:
        encoded = self.END
        encoded += content.dgram.replace(self.ESC, self.ESC + self.ESC_ESC).replace(self.END, self.ESC + self.ESC_END)
        encoded += self.END
        self.ser.write(encoded)

    def receive(self):
        buffer = b''
        while True:
            c = self.ser.read(1)
            if c is None:
                return None

            if c == self.END:
                if len(buffer):
                    break
                continue

            if c == self.ESC:
                c = self.ser.read(1)
                if c is None:
                    return None
                if c == self.ESC_END:
                    buffer += self.END
                elif c == self.ESC_ESC:
                    buffer += self.ESC
            else:
                buffer += c

        return OscMessage(buffer)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('device')
    parser.add_argument('--set', action='append', help='Set crosspoint gain in SRC-DST:GAIN format')
    parser.add_argument('--show', action='store_true', help='Display mixing matrix after adjustments')
    parser.add_argument('--highpass', action='append', help='Set channel highpass frequency in SRC:HZ format')
    parser.add_argument('--gate', action='append', help='Set channel gate threshold in SRC:DBFS format')
    args = parser.parse_args()

    inputs = ['IN1', 'IN2', 'IN3', 'PC', 'USB1', 'USB2']
    outputs = ['OUT1', 'OUT2', 'HP1', 'HP2', 'USB1', 'USB2']
    highpass = []
    gate = []

    client = SLIPClient(args.device, 1152000)

    for ch in range(0, 6):
        message = OscMessageBuilder(f"/ch/{ch}/config/name").build()
        client.send(message)
        response = client.receive()
        inputs[ch] = response.params[0]

        message = OscMessageBuilder(f"/ch/{ch}/eq/1/f").build()
        client.send(message)
        response = client.receive()
        highpass.append(response.params[0])
        
        message = OscMessageBuilder(f"/ch/{ch}/gate/thr").build()
        client.send(message)
        response = client.receive()
        gate.append(response.params[0])
    for ch in range(0, 6):
        message = OscMessageBuilder(f"/bus/{ch}/config/name").build()
        client.send(message)
        response = client.receive()
        outputs[ch] = response.params[0]

    writing = False
    if args.set is not None:
        writing = True
        for change in args.set:
            crosspoint, gain = change.split(':', maxsplit=1)
            src, dst = crosspoint.split('-', maxsplit=1)
            channel = inputs.index(src)
            bus = outputs.index(dst)
            gain = float(gain)

            message = OscMessageBuilder(f"/ch/{channel}/mix/{bus}/level")
            message.add_arg(gain)
            client.send(message.build())
    if args.highpass is not None:
        writing = True
        for change in args.highpass:
            src, f = change.split(':', maxsplit=1)
            channel = inputs.index(src)
            f = float(f)
            message = OscMessageBuilder(f"/ch/{channel}/eq/1/f")
            message.add_arg(f)
            client.send(message.build())
    if args.gate is not None:
        writing = True
        for change in args.gate:
            src, dbfs = change.split(':', maxsplit=1)
            channel = inputs.index(src)
            dbfs = float(dbfs)
            message = OscMessageBuilder(f"/ch/{channel}/gate/thr")
            message.add_arg(dbfs)
            client.send(message.build())

    if writing and not args.show:
            return

    result = []
    head = ['#', '|', 'Highpass','Gate', '|'] + outputs

    for ch in range(0, 6):
        row = [
                inputs[ch],
                '|',
                f"{int(highpass[ch])} Hz" if highpass[ch]>0 else '.',
                f"{gate[ch]} dBfs" if gate[ch] != 0 else '.',
                '|',
            ]
        for bus in range(0, 6):
            message = OscMessageBuilder(f"/ch/{ch}/mix/{bus}/level").build()
            client.send(message)
            response = client.receive()
            level = response.params[0]
            row.append(level if level > 0 else '.')
        result.append(row)
    print(tabulate.tabulate(result, headers=head))


if __name__ == '__main__':
    main()

