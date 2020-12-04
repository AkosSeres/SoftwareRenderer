/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#ifndef _CCANVAS_MATH_POINT_
#define _CCANVAS_MATH_POINT_

#include <math.h>
#include <stdbool.h>

// A struct that represents a point in 2D space
typedef struct {
  double x;
  double y;
} Point;

Point Point_new(double x, double y);
double Point_dist(Point* p1, Point* p2);
double Point_distSq(Point* p1, Point* p2);
bool Point_equals(Point* p1, Point* p2);

#endif