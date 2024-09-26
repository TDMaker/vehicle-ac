#ifndef __RUSC_H__
#define __RUSC_H__
#include <pbc/pbc.h>
#include "lsss.h"
#define ATTR_SET_SIZE 5


void sys_init(element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *pk_frag);
void init_SK(element_t **r_, element_t *K_0, element_t *K_1, element_t **K_2, element_t **K_3, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w);
#endif /* __RUSC_H__ */