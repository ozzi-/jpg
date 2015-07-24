#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "defines.h"
#include "functions.c"

unsigned char *buffer;

struct command {
    int code; 
    int length;
    int index;  // TODO fix to long
}commands[1000];


void print_buffer(int start,int count,bool hex){
	int index;
	for(index=start; index<start+count;index++){
		unsigned char c = (unsigned char)buffer[index];
		if(hex){
			printf("%02X ",c);
		}else{
			printf("%c ",c);
		}
	}
}

int getFileLength(FILE *file){
	fseek(file, 0, SEEK_END);
	int file_len=ftell(file);
	fseek(file, 0, SEEK_SET);
	return file_len;
}

int getDesiredCharsPerLine(int argc, char * argv[]){
	int chars_per_line=12;
	if(argc>2){
		int desired_chars_per_line = atoi(argv[2]);
		if (desired_chars_per_line>0 && desired_chars_per_line<1000){
			return desired_chars_per_line;
		}
	}
	return chars_per_line;
}

void check_integrity(int command_counter,long file_len){
	int offset=0;
	int absolute=0;
	int commands_index;
	struct command current_command;
	struct command next_command;
	bool last_command_is_current;

	for(commands_index=0; commands_index<command_counter; commands_index++){
		current_command=commands[commands_index];

		if(commands_index<(command_counter-1)){
			last_command_is_current=false;
			next_command=commands[(commands_index+1)];
		}else{
			last_command_is_current=true;
		}
		
		if(current_command.code==JPEG_END_OF_IMAGE){
			int end_offset= (current_command.index)+2;
			if((current_command.index+2)!=file_len){
				printf("Data found after the 'End of Image' Command! \nEnd of Image at: %i , actual file length: %lu\n",end_offset,file_len);
				printf("HEX: ");print_buffer(end_offset,file_len-end_offset,true);
				printf("\nDEC: ");print_buffer(end_offset,file_len-end_offset,false);
			}
		}else if(current_command.code==JPEG_START_OF_IMAGE) {
			if(current_command.index!=0){
				printf("Data found before the 'Start of Image' Command!\n");
				printf("HEX: ");print_buffer(0,current_command.index,true);
				printf("\nDEC: ");print_buffer(0,current_command.index,false);
			}
			absolute=absolute+2;
		} else if(current_command.code==JPEG_DEF_RESTART_INTV) {
			if(next_command.index>(current_command.index+4)){
				printf("Unexpected data found after 'Define restart interval' Command!");
				// TODO print till next command index
			}
			absolute=absolute+2;
		} else if(current_command.code==JPEG_START_OF_SCAN) {
			if(next_command.index<commands[commands_index].length){
				printf("Unexpected data found after 'Start of Scan' Command!");
				// TODO print till next command index
			}
			absolute=absolute+next_command.index;
		}else{
			offset =  commands[commands_index].index + commands[commands_index].length;
			if(offset<next_command.index){
				printf("Additional data found after command(FF %i)!\n",current_command.code);
				printf("%i - %i",offset,next_command.index);
			}
			if(offset>next_command.index){
				printf("Missing data found after command(FF %i)!\n",current_command.code);
				printf("%i - %i",offset,next_command.index);
			}
		}
	}
	// CHECK NO STARt, NO END ETC
	// PRINT COMMENTS
	printf("\nDone\n");
}

int main (int argc , char * argv [] ){

	int chars_per_line= getDesiredCharsPerLine(argc, argv);
	FILE *file;
	unsigned long file_len;
	file = fopen(argv[1], "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s\n", argv[1]);
		return 1;
	}
	file_len=getFileLength(file);
	buffer=(char *)malloc(file_len);
	if (!buffer)
	{
		fprintf(stderr, "Memory error!");
		fclose(file);
		return 1;
	}
	fread(buffer,file_len,sizeof(unsigned char),file);
	fclose(file);


	int buffer_index=0;
	int current_line=0;
	int line_counter;
	int command_counter=0;

	int char_cur_i;
	int char_two_i;
	int char_three_i;
	int char_four_i;


	while (buffer_index < file_len){ // READING OVER EDGE OF MEMORY ... TODO !
		unsigned char char_cur   = (unsigned char)buffer[buffer_index]; 
		unsigned char char_two   = (unsigned char)buffer[(buffer_index+1)];
		unsigned char char_three = (unsigned char)buffer[(buffer_index+2)]; 
		unsigned char char_four  = (unsigned char)buffer[(buffer_index+3)];
		char_cur_i = char_cur;
		char_two_i = char_two;
		char_three_i = char_three;
		char_four_i = char_four;
		if(char_cur_i == 255 && char_two_i != 0){
			int command_length = char_three_i*256 + char_four_i;
			struct command c = {char_two_i, command_length+2,buffer_index};
			commands[command_counter++]=c;
		}
		buffer_index++;
	}

	int commands_index;
	for(commands_index=0; commands_index<command_counter; commands_index++){
		//printf("@%i = %i   ",commands[commands_index].index,commands[commands_index].length);
	}

	check_integrity(command_counter,file_len);
	
	printf( RESET"\n" );
	return 0;
}
