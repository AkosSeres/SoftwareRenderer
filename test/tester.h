#ifndef TESTER_FRAMEWORK
#define TESTER_FRAMEWORK
#include <stdbool.h>
#include <math.h>

#define toString2(X) #X
#define toString(X) toString2(X)

#define tester_init() unsigned int errCode;
#define eval(func) errCode = func(); if(errCode != 0) fprintf(stderr, ""toString(func)" failed with code %d\n", errCode); else printf(""toString(func)" succeeded\n");

bool around(double X,double Y, double treshold) {
    return fabs(X-Y) < treshold;
}

#endif