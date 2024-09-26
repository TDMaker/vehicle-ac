#include "rusc.h"
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
pairing_t pairing;

int init_pairing()
{
    FILE *fp = NULL;
    if ((fp = fopen("a.param", "r")) == NULL)
        return -1;
    size_t count = fread(buffer, 1, BUFFER_SIZE, fp);
    fclose(fp);
    fp = NULL;
    pairing_init_set_buf(pairing, buffer, count);
    return 0;
}

void sys_init(element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, element_t *pk_frag)
{
    if (init_pairing() == -1)
    {
        puts("Pairing inits failed!\n");
        exit(-1);
    }
    element_init_G1(*g, pairing);
    element_init_G1(*u, pairing);
    element_init_G1(*h, pairing);
    element_init_G1(*w, pairing);
    element_init_G1(*v, pairing);
    element_init_Zr(*alpha, pairing);
    element_init_GT(*pk_frag, pairing); // pk_frag = e(g,g)^\alpha

    element_random(*g);
    element_random(*u);
    element_random(*h);
    element_random(*w);
    element_random(*v);
    element_random(*alpha);
    pairing_apply(*pk_frag, *g, *g, pairing);
    element_pow_zn(*pk_frag, *pk_frag, *alpha);
    // MK = \alpha
    // PK = (G, G_T, e, g, u, h, w, v, pk_frag)
}

void init_SK(element_t **r_, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w)
{
    *r_ = (element_t *)malloc(sizeof(element_t) * (ATTR_SET_SIZE + 1));
    *K2_ = (element_t *)malloc(sizeof(element_t) * ATTR_SET_SIZE);
    *K3_ = (element_t *)malloc(sizeof(element_t) * ATTR_SET_SIZE);

    element_init_Zr((*r_)[0], pairing);
    element_random((*r_)[0]);
    element_t invert_r, v2invert_r, i_mp;
    element_init_Zr(invert_r, pairing);
    element_invert(invert_r, (*r_)[0]);
    element_init_G1(v2invert_r, pairing);
    element_pow_zn(v2invert_r, *v, invert_r);
    element_init_Zr(i_mp, pairing);

    for (int i = 0; i < ATTR_SET_SIZE; i++)
    {
        element_init_Zr((*r_)[i + 1], pairing);
        element_random((*r_)[i + 1]);
        element_init_G1((*K2_)[i], pairing);
        element_pow_zn((*K2_)[i], *g, (*r_)[i + 1]); //
        element_init_G1((*K3_)[i], pairing);
        element_set_si(i_mp, i + 1);
        element_pow_zn((*K3_)[i], *u, i_mp);
        element_mul((*K3_)[i], (*K3_)[i], *h);
        element_pow_zn((*K3_)[i], (*K3_)[i], (*r_)[i + 1]);
        element_mul((*K3_)[i], (*K3_)[i], v2invert_r);
    }

    K0 = (element_t *)malloc(sizeof(element_t));
    element_init_G1(*K0, pairing);
    K1 = (element_t *)malloc(sizeof(element_t));
    element_init_G1(*K1, pairing);
    element_t tmp_exp;
    element_init_G1(tmp_exp, pairing);
    element_pow_zn(*K0, *g, *alpha);
    element_pow_zn(tmp_exp, *w, (*r_)[0]);
    element_mul(*K0, *K0, tmp_exp);
    element_pow_zn(*K1, *g, (*r_)[0]);

    element_clear(invert_r);
    element_clear(v2invert_r);
    element_clear(i_mp);
    element_clear(tmp_exp);
}