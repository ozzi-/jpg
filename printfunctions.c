extern int output_mode;
extern int chars_per_line;
extern unsigned char *buffer;


void print_buffer(int start,int count,bool hex){
	int index;
	int rest;
	for(index=start; index<start+count;index++){
		unsigned char c = (unsigned char)buffer[index];
		rest=index%chars_per_line;
		if(rest==0){
			printf("\n");
		}
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
			printf(BOLDWHITE"DEC: "RESET);print_buffer(start,count,false);printf("\n");
			printf(BOLDWHITE"HEX: "RESET);print_buffer(start,count,true);;printf("\n");  break;
		case 2:
			printf(BOLDWHITE"HEX: "RESET);print_buffer(start,count,true);;printf("\n");  break;
		case 1:
			printf(BOLDWHITE"DEC: "RESET);print_buffer(start,count,false);;printf("\n"); break;
	}
}


void print_programm_usage(){
	printf(BOLDWHITE"Usage: "RESET);
	printf("jpg filename.jpg {om} {le}\n");
	printf("{om}= Output Mode. 0=No output, 1=ASCII, 2=HEX, 3=ASCII & HEX\n");
	printf("{le}= Number of chars per line\n\n"RESET);
}

void set_command_color(int c){
	switch(c){
		case 192: 
			printf(MAGENTA);
			break;
		case 194: 
			printf(MAGENTA);
			break;
		case 196:
			printf(CYAN);
			break;
		case 208 ... 215:
			printf(GREEN);
		case 216: 
			printf(YELLOW);
			break;
		case 217: 
			printf(MAGENTA);
		case 218: 
			printf(WHITE);
			break;
		case 219: 
			printf(YELLOW);
			break;
		case 221: 
			printf(CYAN);
			break;
		case 224 ... 239: 
			printf(RED);
			break;
		case 254:
			printf(BLUE);
			break;
	}
}



void print_command(int c){
	switch(c){
		case 192: 
			printf("SOF0 - Start Of Frame (Baseline DCT)");
			break;
		case 194: 
			printf("SOF2 - Start Of Frame (Progressive DCT)");
			break;
		case 196:
			printf("DHT - Define Huffman Tables");
			break;
		case 208 ... 215:
			// TODO
		case 216:  
			printf("SOI - Start of image");
			break;
		case 217: 
			printf("EOI - End Of Image");
		case 218: 
			printf("SOS - Start of Scan");
			break;
		case 219: 
			printf("DQT - Define Quantization Table(s)");
			break;
		case 221: 
			printf("DRI - Define Restart Interval");
			break;
		case 224 ... 239: 
			printf("APPn - Applicationspecific");
			break;
		case 254:
			break;
	}
} 
