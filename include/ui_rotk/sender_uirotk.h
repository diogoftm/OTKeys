#ifndef QOT_SENDER_H
#define QOT_SENDER_H

#ifndef KMS_URI
#define KMS_URI "127.0.0.1:8443"
#endif

#ifndef ROOT_CA
#define ROOT_CA "root.crt"
#endif

#ifndef SENDER_SAE_CRT
#define SENDER_SAE_CRT "sae.crt"
#endif

#ifndef SENDER_SAE_KEY
#define SENDER_SAE_KEY "sae.key"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>

#define KEY_LENGTH 512
#define KEY_MEM_SIZE 128
#define OUTPUT_LENGTH 128

	struct qot_sender
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

		char *ksid;
		int key_index;

		unsigned int sender_OTkey[KEY_LENGTH];
	};

	typedef struct qot_sender OKDOT_SENDER;

	void sender_okd(OKDOT_SENDER *);																															   // call OKD service and read the output key
	void sender_output(OKDOT_SENDER *, unsigned long long int *, unsigned long long int *, unsigned int *, unsigned int *, unsigned char (*)[OUTPUT_LENGTH / 32]); // sample hash functions and set sender output
	void deleteLine(FILE *srcFile, FILE *tempFile, const int line);

#ifdef __cplusplus
}
#endif

#endif // ifndef OT_SENDER_H
