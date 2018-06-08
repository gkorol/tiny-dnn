#!/usr/bin/python

#
# Reads a big text file to a multiple c headers
#

import sys

if len(sys.argv) < 6:
    print("Too few arguments. < HEIGHT > < WIDTH > < DEPTH > < array name > < text file in > < prefix name file out >")
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
last_line = 0

for n in height:
    file_w = open(sys.argv[6]+n+".h","w")
    file_w.write("extern const float "+array+"_"+n+"["+str(size)+"] = { \\\n")

    for line in file_r:
        in_line = line + last_line
        if(i == size):
            file_w.write(in_line.rstrip() + " };\n")
        else:
            file_w.write(in_line.rstrip() + ", \\\n")
        i = i + 1
        last_line = line

file_w.close()
file_r.close()
