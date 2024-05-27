KMS_URI = '"127.0.0.1:25575"'
SENDER_SAE_ID = '"qkd//app1@aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa"'
SENDER_SAE_CRT = '"ssl/127.0.0.1.pem"'
SENDER_SAE_KEY = '"ssl/127.0.0.1.key"'
# In most cases the identity of the sender and receiver is the same
RECEIVER_SAE_ID = '"qkd//app2@bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb"'
RECEIVER_SAE_CRT = '"ssl/127.0.0.1.pem"'
RECEIVER_SAE_KEY = '"ssl/127.0.0.1.key"'

CC = /usr/bin/gcc
CFLAGS = -g -w -O3 -Wall -Wextra -fPIC -DKMS_URI=$(KMS_URI) -DROOT_CA=$(ROOT_CA) -DSENDER_SAE_CRT=$(SENDER_SAE_CRT) -DSENDER_SAE_KEY=$(SENDER_SAE_KEY) -DRECEIVER_SAE_CRT=$(RECEIVER_SAE_CRT) -DRECEIVER_SAE_KEY=$(RECEIVER_SAE_KEY) -DSENDER_SAE_ID=$(SENDER_SAE_ID) -DRECEIVER_SAE_ID=$(RECEIVER_SAE_ID)
AS = $(CC) $(CFLAGS) -c
LDFLAGS = -lcrypto -lssl -luuid -lexplain

AR = ar

OBJS_UIROTK = obj/sender_uirotk.o obj/receiver_uirotk.o obj/main_uirotk.o obj/copy.o obj/interfaces.o obj/serializers.o obj/sizes.o obj/ssl_socket.o obj/ssl_socket_client.o
DEPS_UIROTK = include/ui_rotk/sender_uirotk.h include/ui_rotk/receiver_uirotk.h include/etsi_004/copy.h include/etsi_004/interfaces.h include/etsi_004/models.h include/etsi_004/serializers.h include/etsi_004/sizes.h include/etsi_004/ssl_socket_client.h include/etsi_004/ssl_socket.h include/etsi_004/utils.h

all: setup uirotk_test libuirotk

setup:
	test -d obj || mkdir obj

libuirotk: lib $(OBJS_UIROTK)
	$(AR) -crs lib/libuirotk.a $(OBJS_UIROTK)

lib:
	test -d lib || mkdir lib

uirotk_test: $(OBJS_UIROTK)
	$(CC) -no-pie $(CFLAGS) -o $@ $^ $(LDFLAGS)

obj/%_uirotk.o: src/ui_rotk/%_uirotk.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/utils.o: src/qkd/utils.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/copy.o: src/qkd/copy.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/interfaces.o: src/qkd/interfaces.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/serializers.o: src/qkd/serializers.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/sizes.o: src/qkd/sizes.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/ssl_socket.o: src/qkd/ssl_socket.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

obj/ssl_socket_client.o: src/qkd/ssl_socket_client.c $(DEPS_UIROTK)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean

clean:
	-rm -f uirotk_test
	-rm -f obj/*
	-rm -f lib/*
