#include <math.h>
#include <vec3.h>

/**
 * Creates a new 3-dimensional vector
 */
Vec3 Vec3_new(double _x, double _y, double _z) {
  Vec3 v;
  v.x = _x;
  v.y = _y;
  v.z = _z;
  return v;
}

/**
 * Copies the value of a vector into a new variable and returrns it
 */
Vec3 Vec3_copy(Vec3 *src) {
  Vec3 ret;
  ret.x = src->x;
  ret.y = src->y;
  ret.z = src->z;
  return ret;
}

/**
 * Compares two vectors and returns true if they are the same
 */
bool Vec3_equals(Vec3 *v, Vec3 *w) {
  return (v->x == w->x) && (v->y == w->y) && (v->z == w->z);
}

/**
 * Returns the length (magnitude) of the given vector
 */
double Vec3_length(Vec3 *v) {
  return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

/**
 * Returns the length of the given vector squared
 * A lot faster than getting the actual length
 * Useful for comparing two vetors' lengths
 */
double Vec3_sqLength(Vec3 *v) {
  return (v->x * v->x + v->y * v->y + v->z * v->z);
}

/**
 * Multiplies the vector by the given number
 */
void Vec3_mult(Vec3 *v, double num) {
  v->x *= num;
  v->y *= num;
  v->z *= num;
}

/**
 * Divides the vector by the given number
 * When divided by 0, nothing happens
 */
void Vec3_div(Vec3 *v, double num) {
  if (num == 0) return;
  v->x /= num;
  v->y /= num;
  v->z /= num;
}

/**
 * Returns the dot product of the given vectors
 */
double Vec3_dot(Vec3 *v1, Vec3 *v2) {
  return v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
}

/**
 * Returns the square of the dot product of the given vectors
 */
double Vec3_dotSq(Vec3 *v1, Vec3 *v2) { return pow(Vec3_dot(v1, v2), 2); }

/**
 * Adds the second vector to the first one
 */
void Vec3_add(Vec3 *v1, Vec3 *v2) {
  v1->x += v2->x;
  v1->y += v2->y;
  v1->z += v2->z;
}

/**
 * Subtracts the second vector from the first one
 */
void Vec3_sub(Vec3 *v1, Vec3 *v2) {
  v1->x -= v2->x;
  v1->y -= v2->y;
  v1->z -= v2->z;
}

/**
 * Calculates the angle between the given vectors
 * Uses their dot products and acos to calculate it
 * The returned angle is in radians
 * dotSq is used here to save a very expensive sqrt call
 */
double Vec3_angle(Vec3 *v, Vec3 *w) {
  return acos(sqrt((Vec3_dotSq(v, w)) / (Vec3_sqLength(v) * Vec3_sqLength(w))));
}

/**
 * Linearly interpolates between two given vectors and returns the newly created
 * one If t = 1 then the returned vector equals v2 And if t = 0 the returned
 * vector equals v1 If t is outside [0, 1], the function is extrapolating
 */
Vec3 Vec3_lerp(Vec3 *v1, Vec3 *v2, double t) {
  Vec3 ret = Vec3_copy(v1);
  ret.x += (v2->x - v1->x) * t;
  ret.y += (v2->y - v1->y) * t;
  ret.z += (v2->z - v1->z) * t;
  return ret;
}