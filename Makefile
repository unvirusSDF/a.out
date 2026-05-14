CC := clang -O3
CFLAGS := -Wall -Wextra -Wno-unknown-warning-option -Wno-old-school-declaration -Wno-comment -pedantic -std=c23
CLIBS := -lc -lncurses -ldl

OBJS := main.o game.o utils.o

a.out: $(OBJS)
	$(CC) $(CLIBS) -o $@ $(OBJS)

main.o: main.c game.h
	$(CC) $(CFLAGS) -c $< -o $@

utils.o: utils.c utils.h
	$(CC) $(CFLAGS) -c $< -o $@

game.o: game.c game.h utils.h
	$(CC) $(CFLAGS) -c $< -o $@
