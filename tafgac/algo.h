#ifndef __ALGO_H__
#define __ALGO_H__
#include "utils.h"

int init_pairing();
rdmat_f gen_M();
void Parameters_Set_Up(element_t *g, element_t *g_1, element_t *g_2, element_t *a, element_t *b, element_t *alpha, element_t *pk_frag);
void Key_Generation(element_t *K[2], rdmat_mp shares, element_t *g, int *attrs_in_ac, int L);
element_t *Data_Encryption(element_t *E_1, element_t *E_2, element_t **E_3, char **ATTRS_IN_CT, element_t *g, element_t *g_1, element_t *g_2, int *zeta_I, int attr_size_in_ct);
element_t *Data_Decryption(element_t *E_1, element_t *E_2, element_t **E_3, element_t *K[2], rdmat_f M, int *zeta_I, int attr_size_in_ct);
void init_zeta_I(int *zeta_I, char **ATTRS_IN_CT, int attr_size_in_ct);

#endif /* __ALGO_H__ */