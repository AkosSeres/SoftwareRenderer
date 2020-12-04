/**
 * Copyright 2020 Ákos Seres
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 */

#include <point.h>

/**
 * Function for creating/initalising a new point struct
 */
Point Point_new(double x, double y) {
  Point retVal;
  retVal.x = x;
  retVal.y = y;
  return retVal;
}

/**
 * Function for calculating the distance between two points
 */
double Point_dist(Point* p1, Point* p2) {
  return sqrt((p1->x - p2->x) * (p1->x - p2->x) +
              (p1->y - p2->y) * (p1->y - p2->y));
}

/**
 * Function for calculating the square of the distance between two points
 */
double Point_distSq(Point* p1, Point* p2) {
  return ((p1->x - p2->x) * (p1->x - p2->x) +
          (p1->y - p2->y) * (p1->y - p2->y));
}

/**
 * Function for checking if the two points represent the same point on the 2D
 * plane or not
 */
bool Point_equals(Point* p1, Point* p2) {
  return (p1->x == p2->x) && (p1->y == p2->y);
}
