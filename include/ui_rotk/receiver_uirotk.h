#ifndef QOT_RECEIVER_H
#define QOT_RECEIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define KEY_LENGTH 512
#define OUTPUT_LENGTH 128
//#define PATH_MAX 8096

struct qot_receiver
{
	int my_num;
	int other_player;
	char trailing_path_to_ok[32];
	unsigned int receiver_OTkey[KEY_LENGTH];
	unsigned int receiver_OTauxkey[KEY_LENGTH];
	unsigned int indexlist[2][KEY_LENGTH/2];
};

typedef struct qot_receiver OKDOT_RECEIVER;

void receiver_okd(OKDOT_RECEIVER *); //call OKD service and read the output key from text file
void receiver_indexlist(OKDOT_RECEIVER *); //define a pair of index lists based on the oblivious keys
void receiver_output(OKDOT_RECEIVER *, unsigned long long int * , unsigned char *); //set receiver output
void deleteLine(FILE *srcFile, FILE *tempFile, const int line);


#ifdef __cplusplus
}
#endif

#endif //ifndef OT_RECEIVER_H
