#include <stdio.h>
int arg1;
char *arg2[];

main(int arg1, char *arg2[]){
while(--arg1 > 0)
	printf("%s%s", *++arg2, (arg1 > 1) ? "\n" : "");
printf("\n");
return 0;}