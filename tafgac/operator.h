#ifndef __OPERATOR_H__
#define __OPERATOR_H__
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
typedef struct
{
    float *elem;
    int rows;
    int cols;
} rdmat_f;

rdmat_f transpose(rdmat_f);
rdmat_f rdmat_f_mul(rdmat_f, rdmat_f);
rdmat_f get_mand(rdmat_f, rdmat_f);
rdmat_f get_mor(rdmat_f, rdmat_f);
rdmat_f gaussian_elimination(rdmat_f);
rdmat_f pick_rows(int, rdmat_f, int *);
void rdmat_f_print(const char *, rdmat_f);
rdmat_f make_rdmat_f(int, int);
void free_rdmat_f(rdmat_f);

#endif /* __OPERATOR_H__ */