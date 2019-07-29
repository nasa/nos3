#!/usr/bin/python

"""Update the checksum of a TLE."""

import sys

def checksum(line):
    s = 0
    for c in line:
        if c == '-':
            s += 1
        elif c.isdigit():
            s += int(c)
    return s % 10

def fix(line):
    return line[:68] + str(checksum(line[:68]))

# Assumes at least 3 lines - line 1 is sat #/sat name/anything, line 2 is TLE line 1, line 3 is TLE line 2, and the rest of the lines are anything you want
lines = sys.stdin.readlines()[:]
sys.stdout.write(lines[0])
sys.stdout.write(fix(lines[1]))
sys.stdout.write("\n")
sys.stdout.write(fix(lines[2]))
sys.stdout.write("\n")
for i in range(3, len(lines)):
  sys.stdout.write(lines[i])

