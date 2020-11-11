#include <stdio.h>
#include <tester.h>
#include <vec3.h>

void Vec3_print(Vec3* v);
unsigned int test_length();
unsigned int test_multDiv();

int main() {
  tester_init();
  eval(test_length);
  eval(test_multDiv);
  return 0;
}

void Vec3_print(Vec3* v) { printf("(%lf %lf %lf)", v->x, v->y, v->z); }

unsigned int test_length() {
  Vec3 v = Vec3_new(0, 0, 0);
  if (Vec3_length(&v) != 0) return 1;
  if (Vec3_sqLength(&v) != 0) return 2;
  v = Vec3_new(1, 0, 0);
  if (Vec3_length(&v) != 1) return 3;
  if (Vec3_sqLength(&v) != 1) return 4;
  v = Vec3_new(0, 1, 0);
  if (Vec3_length(&v) != 1) return 5;
  if (Vec3_sqLength(&v) != 1) return 6;
  v = Vec3_new(0, 0, 1);
  if (Vec3_length(&v) != 1) return 7;
  if (Vec3_sqLength(&v) != 1) return 8;
  v = Vec3_new(0, 0, 1);
  if (Vec3_length(&v) != 1) return 9;
  if (Vec3_sqLength(&v) != 1) return 10;
  v = Vec3_new(1, 43, 223);
  if (!around(Vec3_length(&v), 227.1101, 0.01)) return 11;
  if (Vec3_sqLength(&v) != 51579) return 12;

  return 0;
}

unsigned int test_multDiv() {
  Vec3 v = Vec3_new(0, 0, 0);
  Vec3 zero = Vec3_new(0, 0, 0);
  Vec3_mult(&v, 1234);
  if (!Vec3_equals(&v, &zero)) return 1;
  v = Vec3_new(0, 0, 0);
  Vec3_div(&v, 1234);
  if (!Vec3_equals(&v, &zero)) return 2;
  v = Vec3_new(128, 322, 1000);
  Vec3_mult(&v, 2);
  Vec3 cmp = Vec3_new(256, 644, 2000);
  if (!Vec3_equals(&v, &cmp)) return 3;
  v = Vec3_new(96, 512, 1500);
  Vec3_div(&v, 4);
  cmp = Vec3_new(24, 128, 375);
  if (!Vec3_equals(&v, &cmp)) return 4;
  v = Vec3_new(423423, 631, 9523);
  Vec3_mult(&v, 0);
  cmp = Vec3_new(0, 0, 0);
  if (!Vec3_equals(&v, &cmp)) return 5;
  v = cmp = Vec3_new(234, 241, 532);
  Vec3_mult(&v, 23);
  Vec3_mult(&cmp, 23);
  if (Vec3_length(&v) / Vec3_length(&cmp) != 1) return 6;
  v = cmp = Vec3_new(2, 0, 13);
  Vec3_div(&v, 0);
  if (!Vec3_equals(&v, &cmp)) return 7;

  return 0;
}