#ifndef _CCANVAS_MATH_VEC3_
#define _CCANVAS_MATH_VEC3_

#include <stdbool.h>

/**
 * Structure representing a 3D vector int the
 * cartesian coordinate system
 */
typedef struct {
  double x, y, z;
} Vec3;

Vec3 vec3_new(double _x, double _y, double _z);
Vec3 vec3_copy(Vec3 *src);
bool vec3_equals(Vec3 *v, Vec3 *w);
double vec3_length(Vec3 *v);
double vec3_sqLength(Vec3 *v);
void vec3_mult(Vec3 *v, double num);
void vec3_div(Vec3 *v, double num);
double vec3_dot(Vec3 *v1, Vec3 *v2);
double vec3_dotSq(Vec3 *v1, Vec3 *v2);
void vec3_add(Vec3 *v1, Vec3 *v2);
void vec3_sub(Vec3 *v1, Vec3 *v2);
double vec3_angle(Vec3 *v, Vec3 *w);
Vec3 vec3_lerp(Vec3 *v1, Vec3 *v2, double t);

#endif