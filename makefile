.PHONY: all setup lib libuirotk clean

CC = /usr/bin/gcc
CFLAGS = -g -w -O3 -Wall -Wextra -fPIC -Iinclude \
         -DKMS_URI='"$(KMS_URI)"' \
         -DROOT_CA='"$(ROOT_CA)"' \
         -DSENDER_SAE_CRT='"$(SENDER_SAE_CRT)"' \
         -DSENDER_SAE_KEY='"$(SENDER_SAE_KEY)"' \
         -DRECEIVER_SAE_CRT='"$(RECEIVER_SAE_CRT)"' \
         -DRECEIVER_SAE_KEY='"$(RECEIVER_SAE_KEY)"'

LDFLAGS = -no-pie -lcrypto -lssl -luuid -lexplain -ljansson -lcurl
AR = ar

OBJS_UIROTK = obj/sender_uirotk.o obj/receiver_uirotk.o obj/main_uirotk.o obj/utils.o
DEPS_UIROTK = include/ui_rotk/sender_uirotk.h include/ui_rotk/receiver_uirotk.h include/ui_rotk/utils.h

all: setup uirotk_test libuirotk

setup:
	test -d obj || mkdir obj
	test -d lib || mkdir lib

libuirotk: lib $(OBJS_UIROTK)
	$(AR) -crs lib/libuirotk.a $(OBJS_UIROTK)

uirotk_test: $(OBJS_UIROTK)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%_uirotk.o: src/ui_rotk/%_uirotk.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/utils.o: src/ui_rotk/utils.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f uirotk_test
	-rm -f obj/*
	-rm -f lib/*
