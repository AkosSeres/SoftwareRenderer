#ifndef _CCANVAS_MATH_POINT_
#define _CCANVAS_MATH_POINT_

#include <math.h>
#include <stdbool.h>

typedef struct {
  double x;
  double y;
} Point;

Point Point_new(double x, double y);
double Point_dist(Point* p1, Point* p2);
double Point_distSq(Point* p1, Point* p2);
bool Point_equals(Point* p1, Point* p2);

#endif