#include "../../include/ui_rotk/sender_uirotk.h"
#include <stdlib.h>

void sender_okd (OKDOT_SENDER * s)
{

	/*opening key file and storing the key in the sender structure*/

	FILE *senderfile;
	FILE *tempFile;

	// Build file string:
	int my_num = s->my_num;
    int other_player = s->other_player;
    char sender_path_to_ok[1024] = "";


	// Concatenate the path components into the buffer
    sprintf(sender_path_to_ok, "%skeys/sender_myId%d_otherId%d_uirotk.txt", s->trailing_path_to_ok, my_num, other_player);

	if ((senderfile = fopen(sender_path_to_ok,"r")))
	{
		for(int j = 0; j < 4; j++)
		{// skip first 4 lines
			if(fscanf(senderfile, "%*[^\n]\n")){}
		}

		// Save key to sender struct
		char okey[KEY_LENGTH];
		if (fscanf(senderfile, "%[^\n]", okey) > 0)
		{
				int i = 0;
			while(i<KEY_LENGTH)
			{
				unsigned int okey_uint = (unsigned int)okey[i];
				s->sender_OTkey[i] = okey_uint - 48;
				i++;
			}
		}else
		{
			perror("QOT ERROR: No more sender oblivious keys.\n");
		}
			
	}
	else
		perror("QOT ERROR: failed to open sender oblivious key file.\n");

	//:: TODO :: Improve this system. It consumes a lot.

	// Delete one line
	char sender_path_to_ok_delete_line[1024] = "";

    // Concatenate the path components into the buffer
    sprintf(sender_path_to_ok_delete_line, "%skeys/sender_myId%d_otherId%d_uirotk_tmp.tmp", s->trailing_path_to_ok, my_num, other_player);
	tempFile = fopen(sender_path_to_ok_delete_line, "w");

	if(tempFile == NULL)
	{
		perror("Unnable to create temporary file.\n");
		perror("Please check you have read/write previleges.\n");
		exit(EXIT_FAILURE);
	}

	// Move src file pointer to beginning
	rewind(senderfile);

	// Delete given line from file
	deleteLine(senderfile, tempFile, 5);

	// Close all open files
	fclose(tempFile);
	fclose(senderfile);

	// Delete src file
	if (remove(sender_path_to_ok)) {
		perror("Error deleting sender oblivious key file.\n");
	}

	// Rename temp file as src
	if (rename(sender_path_to_ok_delete_line, sender_path_to_ok)) {
		perror("Error renaming sender oblivious key file.\n");
	}

	
}



void sender_output (OKDOT_SENDER * s, unsigned long long int * v0 , unsigned long long int * v1, unsigned int * indexb, 
		unsigned int * indexb1, unsigned char (*output)[OUTPUT_LENGTH/32])
{
	unsigned long int input32b[KEY_LENGTH/(2*32)] = {0};
	unsigned long int input32b1[KEY_LENGTH/(2*32)] = {0};


	/*converts the binary hash inputs into 32bit ints*/
	for (int i = 0; i < 32; i++)
	{
		for (int j=0; j<KEY_LENGTH/(2*32); j++)
		{
			input32b[j] <<= 1;
			input32b1[j] <<= 1;

			input32b[j] += s->sender_OTkey[indexb[i+j*32]]  - '0';
			input32b1[j] += s->sender_OTkey[indexb1[i+j*32]] - '0';
		}
	}


	/*hashes pairs of ints from the input32b and intput32b1 arrays into another 32bit value, which is then stored in the output array*/
	for (int i=0; i<OUTPUT_LENGTH/32; i++)
	{
		output[0][i] = (unsigned long int)((v0[0+3*i]*input32b[0+2*i] + v0[1+3*i]*input32b[1+2*i] + v0[2+3*i]) >> 32);
		output[1][i] = (unsigned long int)((v1[0+3*i]*input32b1[0+2*i] + v1[1+3*i]*input32b1[1+2*i] + v1[2+3*i]) >> 32); 		
	}

}




void deleteLine(FILE *srcFile, FILE *tempFile, const int line)
{
	int BUFFER_SIZE = KEY_LENGTH*8;
	char buffer[BUFFER_SIZE];
	int count = 1;

	while((fgets(buffer, BUFFER_SIZE, srcFile)) != NULL)
	{
		if(line != count) {
			fputs(buffer, tempFile);
		}
		count++;
	}

}




