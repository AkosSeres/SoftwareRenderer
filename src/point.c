/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <point.h>

Point Point_new(double x, double y) {
  Point retVal;
  retVal.x = x;
  retVal.y = y;
  return retVal;
}

double Point_dist(Point* p1, Point* p2) {
  return sqrt((p1->x - p2->x) * (p1->x - p2->x) +
              (p1->y - p2->y) * (p1->y - p2->y));
}

double Point_distSq(Point* p1, Point* p2) {
  return ((p1->x - p2->x) * (p1->x - p2->x) +
          (p1->y - p2->y) * (p1->y - p2->y));
}

bool Point_equals(Point* p1, Point* p2) {
  return (p1->x == p2->x) && (p1->y == p2->y);
}
