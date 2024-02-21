#include "../../include/ui_rotk/receiver_uirotk.h"
#include <stdlib.h>
//#include <process.h>
#include <string.h>
#include <unistd.h>

void receiver_okd (OKDOT_RECEIVER * r)
{

	/*opening key files and storing the keys in the receiver structure*/

	FILE *receiverfile;
	FILE *tempFile;

	// Build file string:
	int my_num = r->my_num;
    int other_player = r->other_player;
    char receiver_path_to_ok[1024] = "";

    // Concatenate the path components into the buffer
    sprintf(receiver_path_to_ok, "%skeys/receiver_myId%d_otherId%d_uirotk.txt", r->trailing_path_to_ok, my_num, other_player);
	printf(receiver_path_to_ok);

	if ((receiverfile = fopen(receiver_path_to_ok,"r")))
	{
		printf("QOT SUCCESS: receiver oblivious key file successfully opened.\n");
		for(int j = 0; j < 4; j++)
		{// skip first 4 lines
			if(fscanf(receiverfile, "%*[^\n]\n")){}
		}

		char aux_okey[KEY_LENGTH];
		if (fscanf(receiverfile, "%[^\n]", aux_okey) > 0)
		{
			int i = 0;
			while(i<KEY_LENGTH/2)
			{
				unsigned int aux_okey_uint = (unsigned int)aux_okey[2*i];
				unsigned int okey_uint = (unsigned int)aux_okey[2*i + 1];
				if(aux_okey_uint == 48) // If aux_key is zero
				{
					r->receiver_OTauxkey[2*i] = 0; // The first element is known
					r->receiver_OTauxkey[2*i + 1] = 1; // The second element is unkown

					r->receiver_OTkey[2*i] = okey_uint - 48; //Saves the value known by the receiver
					r->receiver_OTkey[2*i + 1] = 1; // Saves 1: meaning it is unkown

				}else{
					r->receiver_OTauxkey[2*i] = 1; // The first element is unknown
					r->receiver_OTauxkey[2*i + 1] = 0; // The second element is known

					r->receiver_OTkey[2*i] = 1; // Saves 1: meaning it is unkown 
					r->receiver_OTkey[2*i + 1] = okey_uint - 48; // Saves the value known by the receiver
				}
				i++;
			}
		}else
		{
			perror("QOT ERROR: No more receiver oblivious keys.\n");
		}
	}
	else
		perror("QOT ERROR: failed to open receiver oblivious key file.\n");

	//:: TODO :: Improve this system. It consumes a lot.

	// Delete one line
	char receiver_path_to_ok_delete_line[1024] = "";

    // Concatenate the path components into the buffer
    sprintf(receiver_path_to_ok_delete_line, "%skeys/receiver_myId%d_otherId%d_uirotk_tmp.tmp", r->trailing_path_to_ok, my_num, other_player);
	tempFile = fopen(receiver_path_to_ok_delete_line, "w");

	if(tempFile == NULL)
	{
		printf("Unnable to create temporary file.\n");
		printf("Please check you have read/write previleges.\n");
		exit(EXIT_FAILURE);
	}

	// Move src file pointer to beginning
	rewind(receiverfile);
	// Delete given line from file
	deleteLine(receiverfile, tempFile, 5);

	// Close all open files
	fclose(tempFile);
	fclose(receiverfile);

	// Delete src file
	if (remove(receiver_path_to_ok)) {
		printf("Error deleting receiver oblivious key file.\n");
	}

	// Rename temp file as src
	if (rename(receiver_path_to_ok_delete_line, receiver_path_to_ok)) {
		printf("Error renaming receiver oblivious key file.\n");
	}

}





void receiver_indexlist (OKDOT_RECEIVER * r)
{
	int j = 0;
	int k = 0;


	/*generate I0 and I1 index lists from the receiver aux key*/
	for (int i = 0; i<KEY_LENGTH; i++)
	{	
		
		if (r->receiver_OTauxkey[i] == 0) // known bit
		{
			r->indexlist[0][j] = i;
			j++;
		}
		else if (r->receiver_OTauxkey[i] == 1) // unkown bit
		{
			r->indexlist[1][k] = i;
			k++;
		}
		else
			printf ("OT ERROR: invalid key character found.\n");
	}

}





void receiver_output (OKDOT_RECEIVER * r, unsigned long long int * vb, unsigned char * output)
{
	unsigned long int input32[KEY_LENGTH/(2*32)] = {0}; 


	/*converts the binary hash inputs into 32bit ints*/
	for (int i=0; i<32; i++)
	{
		for (int j=0; j<KEY_LENGTH/(2*32); j++)
		{	
			input32[j] <<= 1;
			input32[j] += r->receiver_OTkey[r->indexlist[0][i+j*32]] - '0';
		}
	}


	/*hashes pairs of ints from the input32 array into another 32bit value, which is stored in the output array*/
	for (int i=0; i<OUTPUT_LENGTH/32; i++)
	{
		output[i] = (unsigned long int) ((vb[0+3*i]*input32[0+2*i] + vb[1+3*i]*input32[1+2*i] + vb[2+3*i]) >> 32);
	}



}



