# about
this project is a 2D video game written in C

now that the graphics framework is almost finished, let's start working on the game engine

the ui is all contained in 'ui/' and needs some of the headers in the main file, like 'ui.h' and 'type.h' 

## why this name
note : this is a very long story which hard to follow
it all started when I was doig my neovim config, then I add a shortcut to launch executables faster, to do that I bind C-a to execute './a.out' (the default output name of most compilers), then I got no idea for this thing so i choose that instead of 'e' (my usual default naming option)

## logs
the executable produce a text file named 'log' which is a redirection of stderr
this file contains all debug infos and some random things with it

# build
if yout want to try it
both clang and gcc are supported, other compilers where not tested

to build everything just run `make`, and maybe then `rm *.o`
