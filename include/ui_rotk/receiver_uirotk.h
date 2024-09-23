#ifndef QOT_RECEIVER_H
#define QOT_RECEIVER_H

#ifndef KMS_URI
#define KMS_URI "127.0.0.1:8443"
#endif

#ifndef ROOT_CA
#define ROOT_CA "root.crt"
#endif

#ifndef RECEIVER_SAE_CRT
#define RECEIVER_SAE_CRT "sae.crt"
#endif

#ifndef RECEIVER_SAE_KEY
#define RECEIVER_SAE_KEY "sae.key"
#endif

#ifndef RECEIVER_SAE_ID
#define RECEIVER_SAE_ID ""
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#define KEY_LENGTH 512
#define KEY_MEM_SIZE 128
#define OUTPUT_LENGTH 128

	struct qot_receiver
	{
		int my_num;
		char my_ip[16];
		unsigned int my_port;

		int other_player;
		char other_player_ip[16];
		unsigned int other_player_port;
		char other_player_sai_id[64];

		char *mem;
		int counter;

		unsigned int receiver_OTkey[KEY_LENGTH];
		unsigned int receiver_OTauxkey[KEY_LENGTH];
		unsigned int indexlist[2][KEY_LENGTH / 2];

		char* ksid;
		int key_index;
	};

	typedef struct qot_receiver OKDOT_RECEIVER;

	void receiver_okd(OKDOT_RECEIVER *);											   // call OKD service and read the output key from text file
	void receiver_indexlist(OKDOT_RECEIVER *);										   // define a pair of index lists based on the oblivious keys
	void receiver_output(OKDOT_RECEIVER *, unsigned long long int *, unsigned char *); // set receiver output

#ifdef __cplusplus
}
#endif

#endif // ifndef OT_RECEIVER_H
