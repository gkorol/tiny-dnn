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

lines = file_r.readlines()

print len(lines)
# Write all weights to headers
# file_temp = open("temp_file","w")
# for i in range(height_out):
#     for j in range(size):
#         file_temp.write(lines[j*height_out+i])
#
# file_temp.close()



# file_r.close()
# file_r = open("temp_file","r")

i = 1
n = 0
t = 0

# file_w = open(fileout_name+"_"+str(n)+".h","w")

# Write all weights to headers
# while True:
#     line = file_r.readline()
#     if not line:
#         break
#     if( i == 1 ):
#         file_w = open(fileout_name+"_"+str(n)+".h","w")
#         file_w.write("extern const float "+array+"_"+str(n)+"["+str(size)+"] = { \\\n")
#         file_w.write(line.rstrip() + ", \\\n")
#         n = n + 1
#         i = i + 1
#         # file_w.write(str(x) + ", \\\n")
#         # print n
#     elif( i == size ):
#         file_w.write(line.rstrip() + " };\n")
#         file_w.close()
#         i = 1
#     else:
#         file_w.write(line.rstrip() + ", \\\n")
#         i = i + 1
#         # file_w.write(str(x) + ", \\\n")

# # Write the include file for those headers
# file_w = open(fileout_name+"_include_all.h","w")
# for n in range(height_out):
#     file_w.write("#include \""+fileout_name+"_"+str(n)+".h\"\n")


# # Gera o .c
file_w = open("fc_temp.c","w")
for n in range(height_out):
    file_w.write("out["+str(n)+"] = 0.0;\n")
    file_w.write("for ( i_s = 0; i_s < in_size; i_s++) {\n")
    file_w.write("out[" + str(n) + "] += in[i_s] * " + array + "_" + str(n) + "[i_s];\n")
    file_w.write("}\n")
    file_w.write("out["+str(n)+"] += bias["+str(n)+"];")
    file_w.write("\n")

file_r.close()
