#!/bin/bash

############## CORE ##############

eval cc -c main.c -O5 -std=c23
eval cc -c loader.c -O5 -std=c23

eval cc -c core/core.c -O5 -std=c23
eval cc -c core/callback.c -O5 -std=c23

eval cc -x c -c -DIMPL -DCORE ui.h -O5 -std=c23
eval cc -x c -c -DIMPL header.h -O5 -std=c23


eval cc *.o -o a.out
rm *.o # put in comment to have trace of what's left

##############  UI  ##############

eval cc -c ui/ui.c -O5 -fPIC -std=c23
eval cc -c ui/dlc.c -O5 -fPIC -std=c23

eval cc -x c -c -DIMPL ui/decl.h -O5 -std=c23

eval cc -shared *.o -o ui.so -O5 -lncurses -lpthread

rm *.o # same
