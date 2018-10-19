#!/usr/bin/python

#
# Reads a c header to text file
#

import sys

if len(sys.argv) < 6:
    print("Too few arguments. < HEIGHT > < WIDTH > < DEPTH > < .h file in > < text file out >")
    exit()

height = int(sys.argv[1])
width  = int(sys.argv[2])
depth  = int(sys.argv[3])
size = height * width * depth

file_r = open(sys.argv[4],"r")
file_w = open(sys.argv[5],"w")

first = True

print "Reading C header ...\n"
for line in file_r:
    if first:
        first = False
    else:
        if "," in line:
            numb,trash = line.split(",")
            # print numb
            file_w.write(numb + "\n")
        elif "}" in temp:
            numb,trash = line.split("}")
            # print numb
            file_w.write(numb + "\n")

print "Done !\n"
file_w.close()
file_r.close()
