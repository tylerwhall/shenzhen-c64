#!/usr/bin/env python3

import sys

for byte in iter(lambda: sys.stdin.buffer.read(1), b''):
    val = int("{:08b}".format(byte[0])[::-1], 2)
    sys.stdout.buffer.write(bytes([val]))
