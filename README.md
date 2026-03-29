# about
this project is a 2D video game written in C23 (I tried it doesn't works in c11)

now that the graphics framework is almost finished, let's start working on the game engine

the reason why the ui is dynamicaly loaded is because I suck at graphics and this only adds more fun

## why this name
note: this is a very long story which hard to follow

it all started when I was doig my neovim config, then I add a shortcut to launch executables faster, to do that I bind \<C-a\> to execute './a.out' (the default output name of most compilers), then I got no idea for this thing so i choose that instead of 'e' (my usual default naming option)

## logs
the executable produce a text file named 'log' which is a redirection of stderr
this file contains all debug infos and some random things with it

# build
if yout want to try it
both clang and gcc are supported, other compilers where not tested

to build everything just run `make` (optional flagg :'-j4' if you want it to compile in 0.2s instead of 0.5), and maybe then `rm *.o` to remove garbage

# make tyour own ui
the documentation is contain in doc.md, if you look at the function section you got all the procedure that have to be implemented

note that the keybindings are handeleld by the ui, if you want to add keybindings for the ui, all the keycodes above 128 (included) are free to use

all those function needs to be 'extern "C"' if not written in c, which means no name mangling and the structure also needs to have C representation
