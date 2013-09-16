#include <stdio.h>
#include <stdlib.h>

FILE *fp;
char c = 'a';
int i;	//first byte temp storage
int j;	//second byte temp storage
int la; //load address
int bc; //byte count
unsigned char *ins;	//simulated memory buffer

main(int arg1, char *arg2[]){
while(--arg1 > 0)
	fp = fopen(*++arg2, "r");
	//printf("%s%s", *++arg2, (arg1 > 1) ? " " : ""); *++arg2 gives filename

ins = calloc(2,65536);	//instantiate max buffer size
i = getc(fp);	//get first byte of Y86 header
j = getc(fp);	//get second byte of Y86 header

if(i == 0x79 && j == 0x62){	//check for Y86 file format
while(1){	//infinite loops
	i = getc(fp);	//load first byte of load adress
	j = getc(fp);	//load 2nd byte of load adress
	la = i << 8 | j;	//concate both into a single integer decimal value
	if(i == EOF || j == EOF){	//check to see if LA  is not EOF
		break;}		//leave if it is
	i = getc(fp);	//load 1st byte of byte count
	j = getc(fp);	//load 2nd byte of BC
	bc = i << 8 | j;	//concate both into a single integer decimal value
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
	}
for(i = 0; i<(65536*2); i++){ 	//loop through all of memory and print
	if(ins[i] != 0)		//print all non-zero instructions
		printf("%04X : %02X \n",i,ins[i]);
	}	

}
else{ 
printf("This file is not a Y86 object file \n");
}
//printf("%X %X %X %X \n",i,j,k[2],k[3]);
return 0;}