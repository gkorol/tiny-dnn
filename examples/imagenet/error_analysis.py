#!/usr/bin/python

#
#
#

# N is the number of multiplication error rates by image

import sys
import glob
import matplotlib.pyplot as plt

class Image():
    def __init__(self, name, n, top):
        self.name = name
        self.golden = []
        self.error_vals = []
        self.error_top = []
        self.ANE = []                 # Absolute Number of Errors (element-wise)
        self.T5E = []                 # Presence of error in Top5
        self.T1E = []                 # Presence of error in Top1
        for i in top:
            self.golden.append(i)
        for i in range(n):
            self.ANE.append(0)
            self.T5E.append(0)
            self.T5E.append(0)


if len(sys.argv) < 4:
    print("Too few arguments. < N > < input folder > < output folder >")
    exit()

files_names = glob.glob(sys.argv[2]+"*.txt")
N_errors = int(sys.argv[1])

images = []

for n in files_names:
    if "/" in n:
        image,error = ((n.split("/")[1]).split(".")[0]).split("_")
        # print image+" @ "+error
        item = any(x for x in images if x.name == image)
        if item is True:
           # print "Adding to existing image..."
           file_r = open(n,"r")
           top = []
           for l in file_r:
               top.append(int(l.split("=")[0]))
           file_r.close()
           # print "Top: "+str(top)
           for i in images:
               if i.name == image:
                   i.error_vals.append(error)
                   i.error_top.append([j for j in top])
                   # print "Golden: "+str(i.golden)
           del top[:]
        else:
            # print "New image..."
            file_r = open(n,"r")
            top = []
            top_ = []
            for l in file_r:
                top.append(int(l.split("=")[1]))
                top_.append(int(l.split("=")[0]))
            # print "Top: "+str(top)
            images.append(Image(image,N_errors,top))
            images[-1].error_vals.append(error)
            images[-1].error_top.append([j for j in top_])
            # print "Golden: "+str(images[-1].golden)
            del top[:]
            del top_[:]

print "Done reading files... Total of "+str(len(images))+" images.\n"
for i in images:
    print "Name: "+i.name
    print "Golden: "+str(i.golden)
    print "Rate: "+str(i.error_vals)
    for j in i.error_top:
        print "Top5: "+str(j)
    print ""

for i in images:
    e = 0
    for j in i.error_top:
        for k in range(5):
            if i.golden[k] == j[k]:
                pass
            else:
                i.ANE[e] = 1
        e = e + 1

print "\nANE results:"
for i in images:
    for j in range(len(i.ANE)):
        if i.ANE[j] == 1:
            print i.name+" @ "+str(i.error_vals[j])+" got least one class wrong."
        else:
            print i.name+" @ "+str(i.error_vals[j])+" got all classes right."

print "\nMeasuring T5E..."
for i in images:
    e = 0
    for j in i.error_top:
        a = sorted(i.golden)
        b = sorted(j)
        if a == b:
            print i.name+" @ "+str(i.error_vals[e])+" preserved Top5."
        else:
            print i.name+" @ "+str(i.error_vals[e])+" changed Top5."
        e = e + 1

print "\nMeasuring T1E..."
for i in images:
    e = 0
    for j in i.error_top:
        if i.golden[0] == j[0]:
            print i.name+" @ "+str(i.error_vals[e])+" preserved Top1."
        else:
            print i.name+" @ "+str(i.error_vals[e])+" changed Top1."
        e = e + 1
