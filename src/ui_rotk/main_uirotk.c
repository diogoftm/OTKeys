#include "../../include/ui_rotk/receiver_uirotk.h"
#include "../../include/ui_rotk/sender_uirotk.h"

#include <pthread.h>

// Note that for this test to run as expected distinct sender and receiver certificates need to be defined 
// and they must match the SAE ids defined bellow.

void* receiver_thread(void *arg) {
	receiver_okd ((OKDOT_RECEIVER*)arg);
}

void* sender_thread(void *arg) {
	sender_okd ((OKDOT_SENDER*)arg);
}

int main()
{
	OKDOT_RECEIVER r;
	r.my_num = 1;
	r.other_player = 0;
	strcpy(r.other_player_ip,  "127.0.0.1");
	r.other_player_port = 5454;
	strcpy(r.other_player_sai_id, "qkd//app1@aaaaaaaa-aaaa-aaaa-aaaa-aaaaaaaaaaaa");
	strcpy(r.my_ip, "127.0.0.1");
	r.my_port = 5252;

	OKDOT_SENDER s;
	s.my_num = 0;
	s.other_player = 1;
	strcpy(s.other_player_ip,  "127.0.0.1");
	strcpy(s.other_player_sai_id,  "qkd//app2@bbbbbbbb-bbbb-bbbb-bbbb-bbbbbbbbbbbb");
	s.other_player_port = 5252;
	strcpy(s.my_ip,  "127.0.0.1");
	s.my_port = 5454;
	unsigned char sender_out[2][OUTPUT_LENGTH/32]; //array to store sender's output
	unsigned char receiver_out[OUTPUT_LENGTH/32]; //array to store receiver's output

	unsigned char receiver_in = 1; //receiver choice bit

	/*These 64bit values will define the universal hash functions used in the OT. 
	 * For the sake of simplicity, we define them at the beginning of this test program, 
	 * but in an actual application program they should be chosen at random by the sender during the execution of the protocol.
	 * The MASCOT implementation includes a RNG class, which will be used for this purpose */
	unsigned long long int v[2][12];
	v[0][0] = 0x65d200ce55b19ad8L;	
	v[0][1] = 0x4f2162926e40c299L;
	v[0][2] = 0x162dd799029970f8L;
	v[0][3] = 0x68b665e6872bd1f4L;
	v[0][4] = 0xb6cfcf9d79b51db2L;
	v[0][5] = 0x7a2b92ae912898c2L;
	v[0][6] = 0x65d200ce55b19ad8L;	
	v[0][7] = 0x4f2162926e40c299L;
	v[0][8] = 0x162dd799029970f8L;
	v[0][9] = 0x68b665e6872bd1f4L;
	v[0][10] = 0xb6cfcf9d79b51db2L;
	v[0][11] = 0x7a2b92ae912898c2L;


	v[1][0] = 14719996579123725625U;	
	v[1][1] = 10756103212375473669U;
	v[1][2] = 5130163678318669943U;	
	v[1][3] = 11196048602425907627U;	
	v[1][4] = 9565367076951073642U;	
	v[1][5] = 13747040114869837904U;
	v[1][6] = 15796640148607977966U;	
	v[1][7] = 2315405396079678806U;
	v[1][8] = 17633778172154324773U;
	v[1][9] = 4307822930869792043U;
	v[1][10] = 10460695327545550466U;
	v[1][11] = 11419394045892977570U;

	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, sender_thread, (void*)&s);
	pthread_create(&thread2, NULL, receiver_thread, (void*)&r);
    pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	
	receiver_indexlist (&r);

	sender_output (&s, v[0], v[1], r.indexlist[receiver_in], r.indexlist[(receiver_in)^0x1], sender_out);
	
	for (int i=0; i<OUTPUT_LENGTH/32; i++)
		printf ("Sender's output 0: %x   Sender's output 1: %x  \n", sender_out[0][i], sender_out[1][i]);
	printf ("\n\n");


	
	receiver_output (&r, v[receiver_in], receiver_out);

	for (int i=0; i<OUTPUT_LENGTH/32; i++)
		printf ("Receiver's output: %x  \n", receiver_out[i]);
	printf ("\n\n");


	receiver_in = 0;

	pthread_create(&thread1, NULL, sender_thread, (void*)&s);
	pthread_create(&thread2, NULL, receiver_thread, (void*)&r);
    pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	receiver_indexlist (&r);
	
	sender_output (&s, v[0], v[1], r.indexlist[receiver_in], r.indexlist[(receiver_in)^0x1], sender_out);
	for (int i=0; i<OUTPUT_LENGTH/32; i++)
		printf ("Sender's output 0: %x   Sender's output 1: %x  \n", sender_out[0][i], sender_out[1][i]);
	printf ("\n\n");

	receiver_output (&r, v[receiver_in], receiver_out);
	for (int i=0; i<OUTPUT_LENGTH/32; i++)
		printf ("Receiver's output %x  \n", receiver_out[i]);
	printf ("\n\n");

	return 0;
}
