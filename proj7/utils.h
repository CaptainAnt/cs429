#ifndef _utils_
#define _utils_

typedef short Boolean;
#define TRUE 1
#define FALSE 0

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

#endif