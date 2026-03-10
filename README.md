this project is a 2D video game written in C


now that the graphics framework is almost finished, let's start working on the game engine

the ui is all contained in 'ui/' and needs some of the headers in the main file, like 'ui.h' and 'type.h' 

# build
if yout want to try it
both clang and gcc are supported, other compilers where not tested

## to build the ui:
run those in your shell, after that you can also remove all .o file to clean the garbage
```
cc -c ui/ui.c -O3 -fPIC -std=c23
cc -c ui/dlc.c -O3 -fPIC -std=c23

cc -x c -c -DIMPL ui/decl.h -O3 -std=c23

cc -shared *.o -o ui.so -O3 -lncurses -lpthread
```
## to build the core/tests
same as for the ui, the executable name can be changed in the last line replace 'a.out' by whatever you want
```
eval cc -c main.c -O3 -std=c23
eval cc -c loader.c -O3 -std=c23

eval cc -c core/core.c -O3 -std=c23
eval cc -c core/callback.c -O3 -std=c23

eval cc -x c -c -DIMPL -DCORE ui.h -O3 -std=c23
eval cc -x c -c -DIMPL header.h -O3 -std=c23

eval cc *.o -o a.out
```
