#ifndef __RUSC_H__
#define __RUSC_H__
#include <pbc/pbc.h>
#include "lsss.h"
#define ATTR_SET_SIZE 5
typedef struct
{
    int rows;
    int cols;
    element_t *elem;
} rdmat_mp;

void sys_init(element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *pk_frag);
void key_dist(element_t **r_, element_t *K_0, element_t *K_1, element_t **K_2, element_t **K_3, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w);
void policy_init(element_t *C, element_t *C0, element_t **C1, element_t **C2, element_t **C3, element_t *M, element_t **lambda, element_t *g, element_t *h, element_t *pk_frag, element_t *u, element_t *v, element_t *w, char *input);
rdmat_mp make_rdmat_mp(int rows, int cols);
rdmat_mp rdmat_mul_sp_mp(rdmat a, rdmat_mp b);

#endif /* __RUSC_H__ */