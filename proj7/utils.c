#include <stdlib.h>
#include <math.h>
#include "utils.h"

#define eps = .0000001

Boolean Is_Adjacent_Region(struct region *r1, struct region *r2){
	return FALSE;
}


Boolean Is_Adjacent_Polygon(struct polygon *p1, struct polygon *p2){
	return FALSE;
}


Boolean Intersects_Line(struct point *a1, struct point *a2, struct point *b1, struct point *b2){
	return FALSE;
}

point* create_point(double x1, double y1){
	point* p = (point*)malloc(sizeof(struct point));
	p->x = x1;
	p->y= y1;
	return p;
}

line* create_line(point *b, point *e){
	line *l = (line*)malloc(sizeof(struct line));
	l->start = b;
	l->end = e;
	l->slope = ((e->y - b->y)/(e->x - b->x));
	l->yint = l->slope*(-1*b->x) + b->y; 
	return l;
}

region* create_region(char* n){
	region *r = (region*)malloc(sizeof(struct region));
	r->name = n;
	r->maxx = r->minx = r->maxy = r->miny = 0;
	return r;
}

polygon create_poly(){
	polygon p = (polygon)malloc(sizeof(struct polygon));
	p->start = (point*)malloc(sizeof(struct point));
	p.maxx = p.minx = p.maxy = p.miny = 0;
	return p;
}

char *get_bracket(char* wr){
	char *temp = (char*)malloc(200);
	strncpy(temp,wr);
	return strtok(temp," ,abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n\t\r\b");
}

char *get_word(char* wr){
	char *temp = (char*)malloc(200);
	strncpy(temp,wr);
	return strok(temp,",\n\t\r\b");
}

double get_double(char* wr){
	char* end;
	char* num = strtok(wr,(", \n\t\r\b"));
	return strtod(num,end);
}

/*char* getwrd(char* wr){
	char* ret = (char*)malloc(100);
   	char* token = strtok(wr," \n\t\r\b");
   	int i;
   	for(i = 0; i<spot;i++){
		if(i == 2 && token != NULL)
			return ret;
		if(i == 1 && token == NULL)
			return token;
		if(i == spot-1 && token == NULL)
			ret = token;
		else
			strcpy(ret,token);
		token = strtok(NULL," \n\t\r\b");
   }
   return ret;
}*/