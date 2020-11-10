#include <stdio.h>
#include <tester.h>
#include <vec3.h>

void vec3_print(Vec3* v);
unsigned int test_length();
unsigned int test_multDiv();

int main() {
  tester_init();
  eval(test_length);
  eval(test_multDiv);
  return 0;
}

void vec3_print(Vec3* v) { printf("(%lf %lf %lf)", v->x, v->y, v->z); }

unsigned int test_length() {
  Vec3 v = vec3_new(0, 0, 0);
  if (vec3_length(&v) != 0) return 1;
  if (vec3_sqLength(&v) != 0) return 2;
  v = vec3_new(1, 0, 0);
  if (vec3_length(&v) != 1) return 3;
  if (vec3_sqLength(&v) != 1) return 4;
  v = vec3_new(0, 1, 0);
  if (vec3_length(&v) != 1) return 5;
  if (vec3_sqLength(&v) != 1) return 6;
  v = vec3_new(0, 0, 1);
  if (vec3_length(&v) != 1) return 7;
  if (vec3_sqLength(&v) != 1) return 8;
  v = vec3_new(0, 0, 1);
  if (vec3_length(&v) != 1) return 9;
  if (vec3_sqLength(&v) != 1) return 10;
  v = vec3_new(1, 43, 223);
  if (!around(vec3_length(&v), 227.1101, 0.01)) return 11;
  if (vec3_sqLength(&v) != 51579) return 12;

  return 0;
}

unsigned int test_multDiv() {
  Vec3 v = vec3_new(0, 0, 0);
  Vec3 zero = vec3_new(0, 0, 0);
  vec3_mult(&v, 1234);
  if (!vec3_equals(&v, &zero)) return 1;
  v = vec3_new(0, 0, 0);
  vec3_div(&v, 1234);
  if (!vec3_equals(&v, &zero)) return 2;
  v = vec3_new(128, 322, 1000);
  vec3_mult(&v, 2);
  Vec3 cmp = vec3_new(256, 644, 2000);
  if (!vec3_equals(&v, &cmp)) return 3;
  v = vec3_new(96, 512, 1500);
  vec3_div(&v, 4);
  cmp = vec3_new(24, 128, 375);
  if (!vec3_equals(&v, &cmp)) return 4;
  v = vec3_new(423423, 631, 9523);
  vec3_mult(&v, 0);
  cmp = vec3_new(0, 0, 0);
  if (!vec3_equals(&v, &cmp)) return 5;
  v = cmp = vec3_new(234, 241, 532);
  vec3_mult(&v, 23);
  vec3_mult(&cmp, 23);
  if (vec3_length(&v) / vec3_length(&cmp) != 1) return 6;
  v = cmp = vec3_new(2, 0, 13);
  vec3_div(&v, 0);
  if (!vec3_equals(&v, &cmp)) return 7;

  return 0;
}