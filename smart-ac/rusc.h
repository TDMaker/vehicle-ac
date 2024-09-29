#ifndef __RUSC_H__
#define __RUSC_H__
#include "lsss.h"
#include "utils.h"

void sys_init(element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *pk_frag, element_t *alpha);
void policy_init(element_t *C, element_t *C0, element_t **C1, element_t **C2, element_t **C3, element_t *M, element_t **lambda, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *pk_frag, char *input);
void key_dist(element_t *K_0, element_t *K_1, element_t **K_2, element_t **K_3, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, char **my_attr, int my_attr_size);
void verify(element_t *C, element_t *C0, element_t **C1_, element_t **C2_, element_t **C3_, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *M, char **my_attr, int my_attr_size);
void rd_cleanup(element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *alpha, element_t *pk_frag, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *M, element_t *C, element_t *C0, element_t **C1_, element_t **C2_, element_t **C3_, element_t **lambda_);

#endif /* __RUSC_H__ */