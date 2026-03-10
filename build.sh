#!/bin/bash

############## CORE ##############

eval cc -c main.c -O3 -std=c23
eval cc -c loader.c -O3 -std=c23

eval cc -c core/core.c -O3 -std=c23
eval cc -c core/callback.c -O3 -std=c23

eval cc -x c -c -DIMPL -DCORE ui.h -O3 -std=c23
eval cc -x c -c -DIMPL header.h -O3 -std=c23


eval cc *.o -o a.out
rm *.o # put in comment to have trace of what's left

##############  UI  ##############

eval cc -c ui/ui.c -O3 -fPIC -std=c23
eval cc -c ui/dlc.c -O3 -fPIC -std=c23

eval cc -x c -c -DIMPL ui/decl.h -O3 -std=c23

eval cc -shared *.o -o ui.so -O3 -lncurses -lpthread

rm *.o # same
