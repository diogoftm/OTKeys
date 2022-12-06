CC = /usr/bin/gcc
CFLAGS = -g -O3 -Wall -Wextra
AS = $(CC) $(CFLAGS) -c

OBJS_UIROTK = obj/sender_uirotk.o obj/receiver_uirotk.o obj/main_uirotk.o
DEPS_UIROTK = include/ui_rotk/sender_uirotk.h include/ui_rotk/receiver_uirotk.h

###################################################

all: uirotk_test libuirotk

libuirotk: $(OBJS_UIROTK)
	$(AR) -crs lib/libuirotk.a $(OBJS_UIROTK)

uirotk_test: $(OBJS_UIROTK)
	$(CC) $(CFLAGS) -o bin/$@ $^

obj/%_uirotk.o: src/ui_rotk/%_uirotk.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

###################################################

.PHONY: clean

clean:
	-rm -f bin/uirotk_test
	-rm -f obj/*.o
	-rm -f lib/libuirotk.a
