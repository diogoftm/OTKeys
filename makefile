CC = /usr/bin/gcc
CFLAGS = -g -w -O3 -Wall -Wextra -fPIC -DKMS_URI='"$(KMS_URI)"' -DROOT_CA='"$(ROOT_CA)"' -DSENDER_SAE_CRT='"$(SENDER_SAE_CRT)"' -DSENDER_SAE_KEY='"$(SENDER_SAE_KEY)"' -DRECEIVER_SAE_CRT='"$(RECEIVER_SAE_CRT)"' -DRECEIVER_SAE_KEY='"$(RECEIVER_SAE_KEY)"' -DSENDER_STRICT_ROLE='"$(SENDER_STRICT_ROLE)"' -DRECEIVER_STRICT_ROLE='"$(RECEIVER_STRICT_ROLE)"'
AS = $(CC) $(CFLAGS) -c

AR = ar

OBJS_UIROTK = obj/sender_uirotk.o obj/receiver_uirotk.o obj/main_uirotk.o obj/utils.o
DEPS_UIROTK = include/ui_rotk/sender_uirotk.h include/ui_rotk/receiver_uirotk.h include/ui_rotk/utils.h

all: setup uirotk_test libuirotk

setup:
	test -d obj || mkdir obj

libuirotk: lib $(OBJS_UIROTK)
	$(AR) -crs lib/libuirotk.a $(OBJS_UIROTK)

lib:
	test -d lib || mkdir lib

uirotk_test: $(OBJS_UIROTK)
	$(CC) -no-pie $(CFLAGS) -o $@ $^ -lcurl -ljansson -lcrypto

obj/%_uirotk.o: src/ui_rotk/%_uirotk.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/utils.o: src/ui_rotk/utils.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	-rm -f uirotk_test
	-rm -f obj/*
	-rm -f lib/*
