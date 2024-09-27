#ifndef __UTILS_H__
#define __UTILS_H__
#include <math.h>
#include <string.h>
#include <pbc/pbc.h>
typedef struct
{
    int *elem;
    int rows;
    int cols;
} rdmat;
typedef struct
{
    float *elem;
    int rows;
    int cols;
} rdmat_f;
typedef struct
{
    int rows;
    int cols;
    element_t *elem;
} rdmat_mp;

rdmat make_rdmat(int rows, int cols);
rdmat_f make_rdmat_f(int rows, int cols);
rdmat_mp make_rdmat_mp(int rows, int cols);
rdmat_f rdmat_f_mul(rdmat_f a, rdmat_f b);
rdmat_mp rdmat_mul_sp_mp(rdmat, rdmat_mp);
rdmat pick_rows(int count, rdmat a, int *rows);
rdmat_f gaussian_elimination(rdmat a);
rdmat transpose(rdmat a);
void rdmat_print(const char *name, rdmat a);
void rdmat_f_print(const char *name, rdmat_f a);
void free_rdmat_f(rdmat_f a);
// rdmat_mp rdmat_mul_mp_sp(rdmat_mp, rdmat_f);
// rdmat_mp gen_shares_mp(element_t *secret, rdmat_f M);
// rdmat_mp pick_rows_mp(int count, rdmat_mp a, int *rows);
// rdmat_mp transpose_mp(rdmat_mp a);
// void free_rdmat_mp(rdmat_mp);

#endif /* __UTILS_H__ */