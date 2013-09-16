#include <stdio.h>
#include <stdlib.h>

FILE *fp;
unsigned char c = 'a';
int i;	//first byte temp storage
int j;	//second byte temp storage
int la; //load address
int bc; //byte count
unsigned char *ins;	//simulated memory buffer
int k = 0; //loop holder, extra data holder variable
int sm = 0; //small data block end
int bg = 0; //big data block end
char *out[0xB0]; //initialize data holder for out values
int x; //another holder value

main(int arg1, char *arg2[]){
while(--arg1 > 0)
	fp = fopen(*++arg2, "r");
	//printf("%s%s", *++arg2, (arg1 > 1) ? " " : ""); *++arg2 gives filename

//declare all the output strings possible	
out[0x00] = "halt";
out[0x10] = "nop";
out[0x20] = "rrmovl";
out[0x30] = "irmovl";
out[0x40] = "rmmovl";
out[0x50] = "mrmovl";
out[0x80] = "call";
out[0x90] = "ret";
out[0xA0] = "pushl";
out[0xB0] = "popl";
out[0x20] = "rrmovl";
out[0x21] = "cmovle";
out[0x22] = "cmovl";
out[0x23] = "cmove";
out[0x24] = "cmovne";
out[0x25] = "cmovge";
out[0x26] = "cmovg";
out[0x60] = "addl";
out[0x61] = "subl";
out[0x62] = "andl";
out[0x63] = "xorl";
out[0x70] = "jmp";
out[0x71] = "jle";
out[0x72] = "jl";
out[0x73] = "je";
out[0x74] = "jne";
out[0x75] = "jge";
out[0x76] = "jg";

ins = calloc(2,65536);	//instantiate max buffer size
i = getc(fp);	//get first byte of Y86 header
j = getc(fp);	//get second byte of Y86 header

if(i == 0x79 && j == 0x62){	//check for Y86 file format
while(1){	//infinite loops
	i = getc(fp);	//load first byte of load adress
	j = getc(fp);	//load 2nd byte of load adress
	la = i << 8 | j;	//concate both into a single integer decimal value
	sm = la; //save initial load address for printing later
	if(i == EOF || j == EOF){	//check to see if LA  is not EOF
		break;}		//leave if it is
	i = getc(fp);	//load 1st byte of byte count
	j = getc(fp);	//load 2nd byte of BC
	bc = i << 8 | j;	//concate both into a single integer decimal value
	bg = la + bc; //save final load address for printing later
	if(i == EOF || j == EOF){	//check to see if BC is not EOF
		printf("This Y86 file is corrupted \n");
		return 0;}
	for(i = 0; i < bc; i++){	//for loop through all indicated bytes in memory
		c = getc(fp);	//pull out next byte
		ins[la] = c;	//put into memory @ load adress
		la++;			//go to next load adress
		if(c == EOF){	//check for if byte is not EOF
			printf("This Y86 file is corrupted \n");
			return 0;
			}
		}
	j = 0;
	for(i = sm; i<bg; i++){ //puts the four bytes into an int
		x = ins[i]; //save initial ins value
		j = ins[i+3]; //start with 4th farthest value
		for(k = 2; k>=0;k--){ //loop from 3rd farthest value to the first val
			j = j << 8 | ins[i+k]; //shift saved values to the left, add on new
		}
		if(out[x] == NULL) //if no out command for this ins value...
			printf("%04X %02X %08X %*d %s \n",i,ins[i],j,14,j,"invalid");
		else
			printf("%04X %02X %08X %*d %s \n",i,ins[i],j,14,j,out[x]);
		j = 0;
	}
}

//commenting out old code from previous project... yeah archiving!
/*for(i = 0; i<(65536*2); i++){ 	//loop through all of memory and print
	if(ins[i] != 0)		//print all non-zero instructions
		printf("%04X : %02X \n",i,ins[i]);
	}*/	

}
else{ 
printf("This file is not a Y86 object file \n");
}
return 0;}