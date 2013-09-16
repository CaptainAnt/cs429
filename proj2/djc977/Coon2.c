#include <stdio.h>
#include <stdlib.h>

FILE *fp;
char c = 'a';
int i;
int j;
int la;
int bc; //byte count
unsigned char *ins;

main(int arg1, char *arg2[]){
while(--arg1 > 0)
	fp = fopen(*++arg2, "r");
	//printf("%s%s", *++arg2, (arg1 > 1) ? " " : ""); *++arg2 gives filename

ins = calloc(2,65536);
i = getc(fp);
j = getc(fp);

if(i == 0x79 && j == 0x62){
while(1){
	i = getc(fp);
	j = getc(fp);
	la = i << 8 | j;
	if(i == EOF || j == EOF){
		break;}
	i = getc(fp);
	j = getc(fp);
	bc = i << 8 | j;
	if(i == EOF || j == EOF){
		printf("This Y86 file is corrupted \n");
		return 0;}
	for(i = 0; i < bc; i++){
		c = getc(fp);
		ins[la] = c;
		la++;
		if(c == EOF){
			printf("This Y86 file is corrupted \n");
			return 0;
			}
		}	
	}
for(i = 0; i<(65536*2); i++){
	if(ins[i] != 0)
		printf("%04X : %02X \n",i,ins[i]);
	}	

}
else{ 
printf("This file is not a Y86 object file \n");
}
//printf("%X %X %X %X \n",i,j,k[2],k[3]);
return 0;}