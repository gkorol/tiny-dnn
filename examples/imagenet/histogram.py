#!/usr/bin/python

#
# Draw an histogram based on a weights header file
#


import sys
import matplotlib.pyplot as plt

if len(sys.argv) < 6:
    print("Too few arguments. < F_HEIGHT > < F_WIDTH > < DEPTH > < HEIGHT > < header file in >")
    exit()

height = int(sys.argv[1])
width  = int(sys.argv[2])
depth  = int(sys.argv[3])
height_out  = int(sys.argv[4])
file_r = open(sys.argv[5],"r")

size = height * width * depth * height_out

w = []

i = 0
first_line = True
for line in file_r:
    if(i):
        if(i < size):
            a = line.split(",")
            float_a = float(a[0])
            w.append( float_a )
        else:
            a = line.split("}")
            float_a = float(a[0])
            w.append( float_a )

    i = i + 1

print len(w)

plt.hist(w, bins='auto')
plt.title("Weight distribution with 'auto' bins")
plt.show()
