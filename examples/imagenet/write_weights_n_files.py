#!/usr/bin/python

#
# Reads a big text file to a multiple c headers
#

import sys

if len(sys.argv) < 6:
    print("Too few arguments. < F_HEIGHT > < F_WIDTH > < DEPTH > < HEIGHT > < array name > < text file in > < prefix name file out >")
    exit()

height = int(sys.argv[1])
width  = int(sys.argv[2])
depth  = int(sys.argv[3])
height_out  = int(sys.argv[4])
array = sys.argv[5]
file_r = open(sys.argv[6],"r")
fileout_name = sys.argv[7]

size = height * width * depth
hxw = height * width
i = 1
n = 0

# TODO...

for line in file_r:
    if( hxw%i == 0 ):
        n = n + 1
        file_w.write("extern const float "+array+"_"+str(n)+"["+str(size)+"] = { \\\n")

    if( i == size ):
        file_w.write(line.rstrip() + " };\n")
    else:
        file_w.write(line.rstrip() + ", \\\n")

    i = i + 1


file_w.close()
file_r.close()
