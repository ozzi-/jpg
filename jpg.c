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
    long index;
}commands[1000];

int output_mode;


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


void print_buffer_output_mode(int start, int count){
	switch(output_mode){
		case 3: 
			printf("DEC: ");print_buffer(start,count,false);printf("\n");
			printf("HEX: ");print_buffer(start,count,true);;printf("\n");  break;
		case 2:
			printf("HEX: ");print_buffer(start,count,true);;printf("\n");  break;
		case 1:
			printf("DEC: ");print_buffer(start,count,false);;printf("\n"); break;
	}
}

int get_file_length(FILE *file){
	fseek(file, 0, SEEK_END);
	int file_len=ftell(file);
	fseek(file, 0, SEEK_SET);
	return file_len;
}

void load_argv_param(int argc, char * argv[]){
	int i;
	for(i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
}

int get_desired_chars_per_line(int argc, char * argv[]){
	int chars_per_line=default_chars_per_line;
	if(argc>2){
		int desired_chars_per_line = atoi(argv[2]);
		if (desired_chars_per_line>0 && desired_chars_per_line<1000){
			return desired_chars_per_line;
		}
	}
	return chars_per_line;
}

// 0 - None | 1 - ASCII | 2 - HEX | 3 - ALL
void getOutputMode(int argc, char * argv[]){
	output_mode = 1;
	if(argc>3){
		int desired_output_mode = atoi(argv[3]);
		if (desired_output_mode>=0 && desired_output_mode<=3){
			output_mode=desired_output_mode;
		}
	}
}


void check_additional_data_start_end(struct command current_command,long file_len,int command_counter){
	int current_command_eff_index= (current_command.index)+2;
	if(current_command.code==JPEG_END_OF_IMAGE){
		if((current_command.index+2)!=file_len){
			printf(BOLDYELLOW"Unexpected data found after the 'End of Image' Command! \nEnd of Image at: %i , actual file length: %lu\n"RESET,current_command_eff_index,file_len);
			print_buffer_output_mode(current_command_eff_index,file_len-current_command_eff_index);
			printf("\n");
		}
	}else if(current_command.code==JPEG_START_OF_IMAGE) {
		if(current_command.index!=0){
			printf(BOLDYELLOW"Unexpected data found before the 'Start of Image' Command!\n"RESET);
			print_buffer_output_mode(0,current_command.index);
			printf("\n");
		}
	} 
}

void check_if_start_end_existing(int command_counter){
	bool file_has_start=false;
	bool file_has_end=false;
	struct command current_command;
	int commands_index=0;

	for(commands_index=0; commands_index<command_counter; commands_index++){
		current_command=commands[commands_index];
		file_has_end	=((current_command.code==JPEG_END_OF_IMAGE) 	|| file_has_end);
		file_has_start	=((current_command.code==JPEG_START_OF_IMAGE) 	|| file_has_start);
	}

	if(!file_has_start){
		printf(BOLDRED"File contains no start command!\n"RESET);
	}
	if(!file_has_end){
		printf(BOLDRED"File contains no end command!\n"RESET);
	}
}


void check_integrity(int command_counter,long file_len){
	long offset=0;
	long absolute=0;
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
		
		int current_command_eff_index= (current_command.index)+2;
		check_additional_data_start_end(current_command,file_len,command_counter);

		absolute=absolute+current_command.length;

		if(current_command.code==JPEG_DEF_RESTART_INTV) {
			if(next_command.index>(current_command.index+4)){
				printf(BOLDYELLOW"Unexpected data found after 'Define restart interval' Command!\n"RESET);
				print_buffer_output_mode(current_command_eff_index,next_command.index-current_command_eff_index);
			}
			absolute=absolute+2;
		} else if(current_command.code==JPEG_START_OF_SCAN) {
			if(next_command.index<commands[commands_index].length){
				printf(BOLDBLUE"Missing data after 'Start of Scan' Command!\n"RESET);
			}
			// This marker specifies which slice of data it will contain, and is immediately followed by entropy-coded data.
			absolute=absolute - current_command.length;
			absolute=absolute + (next_command.index - current_command.index);
		} else if(current_command.code>=224 && current_command.code <= 239){
			printf(BOLDWHITE"Application specific data found:\n"RESET);
			print_buffer_output_mode(current_command_eff_index,current_command.length);
			printf("\n");
			if(next_command.index!=(commands[commands_index].length)+2){
				printf(BOLDRED"Invalid length given with Application specific data marker!\n"RESET);
				printf("\n%li-%i\n",next_command.index,commands[commands_index].length);
			}
		}else if(current_command.code!= JPEG_START_OF_IMAGE && current_command.code != JPEG_END_OF_IMAGE){
			offset =  commands[commands_index].index + commands[commands_index].length;
			if(offset<next_command.index){
				printf(BOLDYELLOW"Additional data found after command(FF %i)!\nRESET",current_command.code);
				printf("%li - %li",offset,next_command.index);
			}
			if(offset>next_command.index){
				printf(BOLDBLUE"Missing data found after command(FF %i)!\nRESET",current_command.code);
				printf("%li - %li",offset,next_command.index);
			}
		}

		if(current_command.code==JPEG_COMMENT){
			printf(BOLDWHITE"Comment found:\n"RESET);
			print_buffer_output_mode(current_command_eff_index,current_command.length);
			printf("\n");
		}
	}
	// TODO check for absolute

	check_if_start_end_existing(command_counter);
	
	if(absolute!=file_len) {
		printf(BOLDRED"Calculated file length: %li, file length on filesystem: %li are mismatching!"RESET,absolute,file_len);
	}
	printf("\nDone\n");
}

int main (int argc , char * argv [] ){

	int chars_per_line= get_desired_chars_per_line(argc, argv);
	getOutputMode(argc,argv);
	load_argv_param(argc,argv);

	FILE *file;
	unsigned long file_len;
	file = fopen(argv[1], "rb");
	if (!file)
	{
		fprintf(stderr, "Unable to open file %s\n", argv[1]);
		return 1;
	}
	file_len=get_file_length(file);
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


	while (buffer_index < file_len){

		char_cur_i=-1;
		char_two_i=-1;
		char_three_i=-1;
		char_four_i=-1;

		unsigned char char_cur   = (unsigned char)buffer[buffer_index]; 
		char_cur_i = char_cur;

		if((buffer_index+1)!=file_len){
			unsigned char char_two   = (unsigned char)buffer[(buffer_index+1)];
			char_two_i = char_two;
		}
		if((buffer_index+2)!=file_len){
			unsigned char char_three = (unsigned char)buffer[(buffer_index+2)]; 
			char_three_i = char_three;
		}
		if((buffer_index+3)!=file_len){
			unsigned char char_four  = (unsigned char)buffer[(buffer_index+3)];
			char_four_i = char_four;
		}



		if(char_cur_i == 255 && char_two_i != 0){
			int command_length;
			if(char_two_i==JPEG_START_OF_IMAGE || char_two_i==JPEG_END_OF_IMAGE){ // START TAG DOESNT HAVE SIZE MARKERS
				command_length = 2;
			}else{
				command_length = char_three_i*256 + char_four_i+2;	
			}
			struct command c = {char_two_i, command_length,buffer_index};
			commands[command_counter++]=c;
		}
		buffer_index++;
	}

	check_integrity(command_counter,file_len);
	
	printf( RESET"\n" );
	return 0;
}
