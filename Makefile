CC := clang -O3
CFLAGS := -Wall -Wextra -Wno-old-style-declaration -Wno-unknown-warning-option -Wno-comment -pedantic -std=c23


.PHONY:
	all
	clean
	force_build

all: a.out ui.so

clean:
	rm *.o core/*.o ui/*.o

# same as 'make -B'
force_build: clean all

################# CORE ###################

CORE_OBJS:= main.o loader.o core/core.o core/callback.o ui.o header.o
CORE_FLAGS := -DCORE
CORE_LIB :=
CORE_HEADERS := ui.h header.h


a.out: $(CORE_OBJS)
	$(CC) $(CFLAGS) -o a.out $^

main.o: main.c ui.h header.h core/callback.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c main.c -o $@

loader.o: loader.c ui.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -Wno-pedantic -c loader.c

core/core.o: core/core.c header.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c core/core.c -o $@

core/callback.o: core/callback.c core/callback.h types.h header.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c core/callback.c -o $@

ui.o: ui.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -x c -c -DIMPL -DCORE ui.h -o $@

header.o: header.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -x c -c -DIMPL header.h -o $@


################## UI ####################

UI_OBJS := ui/ui.o ui/dlc.o ui/decl.o
UI_LIBS := -lncurses -lpthread
UI_FLAG := -fPIC -Wno-gnu-zero-variadic-macro-arguments -Wno-format-pedantic

ui.so: $(UI_OBJS)
	$(CC) $(UI_LIBS) -shared $^ -o $@

ui/ui.o: ui/ui.c ui.h ui/decl.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c ui/ui.c -o $@

ui/dlc.o: ui/dlc.c ui.h ui/decl.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c ui/dlc.c -o $@

ui/decl.o: ui.h ui/decl.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c -x c ui/decl.h -o $@ -DIMPL
