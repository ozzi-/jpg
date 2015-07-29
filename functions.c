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
