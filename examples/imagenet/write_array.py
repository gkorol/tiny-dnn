#!/usr/bin/python

#
# Reads a text file to a c header
#

import sys

if len(sys.argv) < 6:
    print("Too few arguments. < HEIGHT > < WIDTH > < DEPTH > < array name > < text file in > < .h file out >")
    exit()

height = int(sys.argv[1])
width  = int(sys.argv[2])
depth  = int(sys.argv[3])
size = height * width * depth

array = sys.argv[4]

file_r = open(sys.argv[5],"r")
file_w = open(sys.argv[6],"w")

file_w.write("extern const float " + array + "[" + str(size) + "] = { \\\n")
i = 1

for line in file_r:
    if(i == size):
        file_w.write(line.rstrip() + " };\n")
    else:
        file_w.write(line.rstrip() + ", \\\n")
    i = i + 1

file_w.close()
file_r.close()
