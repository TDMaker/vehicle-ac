#ifndef __UTILS_H__
#define __UTILS_H__
#include "operator.h"
#include <pbc/pbc.h>
typedef struct
{
    int rows;
    int cols;
    element_t *elem;
} rdmat_mp;
rdmat_mp make_rdmat_mp(int, int);
rdmat_mp rdmat_mul_sp_mp(rdmat_f, rdmat_mp);
rdmat_mp rdmat_mul_mp_sp(rdmat_mp, rdmat_f);
rdmat_mp gen_shares_mp(element_t *secret, rdmat_f M);
rdmat_mp pick_rows_mp(int count, rdmat_mp a, int *rows);
rdmat_mp transpose_mp(rdmat_mp a);
void free_rdmat_mp(rdmat_mp);

#endif /* __UTILS_H__ */