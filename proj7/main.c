#include <stdlib.h>
#include <stdio.h>

#if 1
   #define debugPrint(a) printf a
#else
   #define debugPrint(a) (void)0
#endif

typedef struct point{
	double x,y;
}point;

typedef struct line{
	point *start, *end;
	double yint, slope;
}line;

typedef struct polygon{
	line lines[8192];
	point* start;
	double maxx, minx, maxy, miny;
}polygon;

typedef struct region{
	char* name;
	polygon poly[20];
	double maxx, minx, maxy, miny;
	struct region* next;
}region;

/*point* create_point(double x1, double y1){
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
}*/

FILE *fp;
main(int arg1, char *arg2[]){
	char delim[] = {' ', ',', '/n', '/t', '/r', '/0'};
	char buffer[2048];
	while(--arg1 > 0){
		fp = fopen(*++arg2, "r");}
	while (fgets(buffer, 200, fp)){
		char *line = buffer;
		char *token;
		token = strtok(line,delim);
		while(token!=NULL){
			debugPrint((token));
			token = strtok(NULL,delim);
		}
	}
}