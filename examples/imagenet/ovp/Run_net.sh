#!/bin/sh

${IMPERAS_ISS} --verbose --program net_ovp.ARM7-O0-g.elf \
     --processorvendor arm.ovpworld.org --processorname arm --variant Cortex-A5MPx1 \
     --numprocessors 1     --parameter compatibility=nopSVC --parameter UAL=1 --parameter endian=little   "$@"     -argv 39

#${IMPERAS_ISS} --verbose --program net \
#    --processorvendor arm.ovpworld.org --processorname arm --variant Cortex-A5MPx4 \
#     --numprocessors 1 --parameter endian=little "$@" -argv 39
