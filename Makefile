CC := clang -O3
CFLAGS := -Wall -Wextra -Wno-old-style-declaration -Wno-unknown-warning-option -Wno-comment -Wno-gnu-zero-variadic-macro-arguments -pedantic -std=c23

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

CORE_OBJS:= main.o loader.o core/core.o core/callback.o ui.o core/action_heap.o header.o
CORE_FLAGS := -DCORE
CORE_LIB := -ldl
CORE_HEADERS := ui.h header.h


a.out: $(CORE_OBJS)
	$(CC) $(CFLAGS) -o a.out $^

main.o: main.c ui.h header.h core/callback.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c $< -o $@

loader.o: loader.c ui.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -Wno-pedantic -c $<

core/core.o: core/core.c header.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c $< -o $@

core/callback.o: core/callback.c core/callback.h types.h header.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c $< -o $@

core/action_heap.o: core/action_heap.c core/action_heap.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -c $< -o $@

ui.o: ui.h types.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -x c -c -DIMPL -DCORE $< -o $@

header.o: header.h
	$(CC) $(CFLAGS) $(CORE_FLAGS) -x c -c -DIMPL $< -o $@


################## UI ####################

UI_OBJS := ui/ui.o ui/dlc.o ui/decl.o
UI_LIBS := -lncurses -lpthread -ldl
UI_FLAG := -fPIC -Wno-format-pedantic

ui.so: $(UI_OBJS)
	$(CC) $(UI_LIBS) -shared $^ -o $@

ui/ui.o: ui/ui.c ui.h ui/decl.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c $< -o $@

ui/dlc.o: ui/dlc.c ui.h ui/decl.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c $< -o $@

ui/decl.o: ui/decl.h ui.h types.h
	$(CC) $(CFLAGS) $(UI_FLAG) -c -x c $< -o $@ -DIMPL

