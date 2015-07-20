#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#define RESET   "\033[0m"
#define BOLDWHITE   "\033[1m\033[37m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m" 

int last_command=0;
// https://en.wikipedia.org/wiki/JPEG
void setColor(int c){
	last_command=c;
	switch(c){
		case 192: // SOF0 - Start Of Frame (Baseline DCT)
			printf(MAGENTA);
		case 194: // SOF2 - Start Of Frame (Progressive DCT)
			printf(MAGENTA);
		case 196:  // DHT - Define Huffman Tables
			printf(CYAN);
			break;
		case 208:
		case 209:
		case 210:
		case 211:
		case 212:
		case 213:
		case 214:
		case 215:
			printf(GREEN);
		case 216: // SOI - Start of image
			printf(YELLOW);
			break;
		case 217: // EOI - End Of Image
			printf(MAGENTA);
		case 218: // SOS - Start of Scan
			printf(WHITE);
			break;
		case 219: // DQT - Define Quantization Table(s)
			printf(YELLOW);
			break;
		case 221: // DRI - Define Restart Interval
			printf(CYAN);
			break;
		case 224: // APPn - Applicationspecific
		case 225:
		case 226:
		case 227:
		case 228:
		case 229:
		case 230:
		case 231:
		case 232:
		case 233:
		case 234:
		case 235:
		case 236:
		case 237:
		case 238:
		case 239:
			printf(RED);
			break;
		case 254:
			printf(BLUE);
			break;
		default:
			//last_command=?
			break;
	}
}

void printoz(unsigned char cur_char, unsigned char next_char,bool hex){
	setColor(last_command);
	if(hex){
		int cur_char_i = cur_char;
		int next_char_i = next_char;
		if(cur_char_i == 255 && next_char_i != 0){
			setColor(next_char_i);
			last_command= next_char_i;
		}
		printf("%02X ",cur_char);
	}else{
		if(cur_char != 10 && cur_char != 11 && cur_char != 12 && cur_char != 13){
			printf("%c ",cur_char);
		}else{
			printf("\\");
		}
	}
}

int getFileLength(FILE *file){
	fseek(file, 0, SEEK_END);
	int fileLen=ftell(file);
	fseek(file, 0, SEEK_SET);
	return fileLen;
}

int main (int argc , char * argv [] ){

        FILE *file;
        unsigned char *buffer;
        unsigned long fileLen;
        int chars_per_line=12;

        file = fopen(argv[1], "rb");
        if (!file)
        {
                fprintf(stderr, "Unable to open file %s\n", argv[1]);
                return 1;
        }

        if(argc>2){
        	int desired_chars_per_line = atoi(argv[2]);
        	if (desired_chars_per_line>0 && desired_chars_per_line<1000){
        		chars_per_line=desired_chars_per_line;
        	}
        }

        fileLen=getFileLength(file);

        buffer=(char *)malloc(fileLen);
        if (!buffer)
        {
			fprintf(stderr, "Memory error!");
			fclose(file);
			return 1;
        }

       fread(buffer,fileLen,sizeof(unsigned char),file);
       fclose(file);

		int i=0;
		int line_counter=0;
		int line=0;
		int empty_chars=0;
		unsigned char cur_char;
		unsigned char next_char;
		int cur_char_i;
		int next_char_i;

		while (i < fileLen){
			printf(BOLDWHITE"%04i: "RESET,line);
			

			for(line_counter=0;line_counter<chars_per_line;line_counter++){
				if(fileLen-i>0){
					printoz((unsigned char)buffer[i],(unsigned char)buffer[(i+1)],true);
					i++;
				}else{
					printf("   ");
					empty_chars++;
				}
			}
			i=i-chars_per_line+empty_chars;
			printf(" |  ");
			for(line_counter=0;line_counter<chars_per_line;line_counter++){
				if(fileLen-i>0){
					printoz((unsigned char)buffer[i],(unsigned char)buffer[(i+1)],false);
					i++;
				}
			}
			line++;
			printf( "\n" );	
       }
	   printf( RESET"\n" );
       return 0;
}
