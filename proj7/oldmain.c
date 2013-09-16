#include <stdlib.h>
#include <stdio.h>
#include "utils.h"

#if 1
   #define debugPrint(a) printf a
#else
   #define debugPrint(a) (void)0
#endif

FILE *fp;
Boolean rflag = FALSE;	//region flag
Boolean pflag = FALSE;	//polygon flag

main(int arg1, char *arg2[]){
	region* root = (region*)malloc(sizeof(region));					//initial root for linked list of regions
	region* curr_region = (region*)malloc(sizeof(region));			//current region on linked list
	int rcount = 0;													//counter for current region
	int pcount = 0;													//counter for current polygon in region
	int lcount = 0;													//counter for current line in polygon
	polygon curr_poly;
	point* point1 = (point*)malloc(sizeof(point));
	point* point2 = (point*)malloc(sizeof(point));
	debugPrint(("IT LIVES!\n"));
	char* c = (char*)malloc(200);
	char *curr_bracket = (char*)malloc(200);
	while(--arg1 > 0){
		fp = fopen(*++arg2, "r");}
	
	debugPrint(("Entering region/polygon read loop"));
	while (fgets(c, 200, fp) != NULL){
		if(curr_bracket == NULL || curr_bracket[0]==EOF){
			curr_bracket = get_bracket(c);
		}
		if(rflag == FALSE && curr_bracket[0]=='{'){
			rflag = TRUE;
			char* name = get_word(c);
			if(root == NULL){
				curr_region = new_region(name);
				root = curr_region;
			}
			else{
				curr_region->next = new_region(name);
				curr_region = curr_region->next;
			}
		}
		if(pflag == FALSE && curr_bracket[0] == '['){
			pflag = TRUE;
			curr_region->poly[pcount] = create_poly();
			curr_poly = curr_region->poly[pcount];
			curr_bracket = get_bracket(c);
		}
		//parse what's inside the ] bracket (all dem points)
		while(curr_bracket[0] == '('){
			add_line(c,point1,curr_poly);
			if(point1 == NULL){
				double x = get_double(c);
				double y = get_double(c);
				point1 = create_point(x,y);
				curr_bracket = get_bracket(c);
				curr_poly->start = point1;
				if(curr_bracket != ")"){
					debugPrint(("Error in point data"));
					return;
				}
				curr_bracket = get_bracket(c);
			}
			else{
				double x = get_double(c);
				double y = get_double(c);
				point2 = create_point(x,y);
				curr_poly->lines[lcount] = create_line(point1,point2);
				if(point2 == curr_poly->start)
					break;
				point1 = point2;
				curr_bracket = get_bracket(c);
				if(curr_bracket != ")"){
					debugPrint(("Error in point data"));
					return;
				}
				curr_bracket = get_bracket(c);
			}
		}
		if(pflag == TRUE && curr_bracket == "]"){
			pflag == FALSE;
			pcount++;
			lcount = 0;
			point1 = NULL;
			point2 = NULL;
			curr_bracket = get_bracket(c);
		}
		if(rflag == TRUE && curr_bracket == "}"){
			rflag = FALSE;
			rcount ++;
			pcount = 0;
			curr_bracket = get_bracket(c);
		}
	}
	return;
}