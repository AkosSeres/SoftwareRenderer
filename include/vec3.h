#ifndef _CCANVAS_MATH_VEC3_
#define _CCANVAS_MATH_VEC3_

#include <math.h>
#include <stdbool.h>

/**
 * Structure representing a 3D vector int the
 * cartesian coordinate system
 */
typedef struct {
  double x, y, z;
} Vec3;

Vec3 Vec3_new(double _x, double _y, double _z);
Vec3 Vec3_copy(Vec3 *src);
bool Vec3_equals(Vec3 *v, Vec3 *w);
double Vec3_length(Vec3 *v);
double Vec3_sqLength(Vec3 *v);
void Vec3_mult(Vec3 *v, double num);
void Vec3_div(Vec3 *v, double num);
double Vec3_dot(Vec3 *v1, Vec3 *v2);
double Vec3_dotSq(Vec3 *v1, Vec3 *v2);
void Vec3_add(Vec3 *v1, Vec3 *v2);
void Vec3_sub(Vec3 *v1, Vec3 *v2);
double Vec3_angle(Vec3 *v, Vec3 *w);
Vec3 Vec3_lerp(Vec3 *v1, Vec3 *v2, double t);
void Vec3_setLength(Vec3 *v, double lenToSet);
void Vec3_rotateX(Vec3 *v, double angle);
void Vec3_rotateY(Vec3 *v, double angle);
void Vec3_rotateZ(Vec3 *v, double angle);
void Vec3_rotateAroundAxis(Vec3 *v, Vec3 *axis, double angle);
Vec3 Vec3_cross(Vec3* v1, Vec3* v2);

#endif