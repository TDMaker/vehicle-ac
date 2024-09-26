#include "rusc.h"
#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
pairing_t pairing;

rdmat W;
char **rho;

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

void key_dist(element_t **r_, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w)
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
void policy_init(element_t *C, element_t *C0, element_t **C1, element_t **C2, element_t **C3, element_t *M, element_t **lambda, element_t *g, element_t *h, element_t *pk_frag, element_t *u, element_t *v, element_t *w, char *input)
{
    TreeNode *root = get_complete_tree(input);
    breadth_first_traversal(root, display);
    get_W_rho(&W, &rho, root);

    element_init_GT(*M, pairing);
    element_random(*M);
    int L = W.rows;
    rdmat_mp vec_v = make_rdmat_mp(L, 1);
    for (int i = 0; i < W.cols; i++)
    {
        element_random(vec_v.elem[i]);
    }

    *lambda = rdmat_mul_sp_mp(W, vec_v).elem;
    rdmat_mp t_ = make_rdmat_mp(1, L);
    for (int i = 0; i < L; i++)
    {
        element_random(t_.elem[i]);
    }
    element_init_GT(*C, pairing);
    element_mul(*C, *M, *pk_frag);
    element_mul_zn(*C, *C, vec_v.elem[0]);
    element_init_G1(*C0, pairing);
    element_pow_zn(*C0, *g, vec_v.elem[0]);
    *C1 = (element_t *)malloc(sizeof(element_t) * L);
    *C2 = (element_t *)malloc(sizeof(element_t) * L);
    *C3 = (element_t *)malloc(sizeof(element_t) * L);
    element_t tmp1, tmp2;
    element_init_G1(tmp1, pairing);
    element_init_Zr(tmp2, pairing);
    for (int i = 0; i < L; i++)
    {
        element_init_G1((*C1)[i], pairing);
        element_pow_zn((*C1)[i], *w, (*lambda)[i]);
        element_pow_zn(tmp1, *v, t_.elem[i]);
        element_mul((*C1)[i], (*C1)[i], tmp1);

        element_init_G1((*C2)[i], pairing);
        element_from_hash(tmp2, rho[i], strlen(rho[i]));
        element_pow_zn((*C2)[i], *u, tmp2);
        element_mul((*C2)[i], (*C2)[i], *h);
        element_invert(tmp2, t_.elem[i]);
        element_pow_zn((*C2)[i], (*C2)[i], tmp2);

        element_init_G1((*C3)[i], pairing);
        element_pow_zn((*C3)[i], *g, t_.elem[i]);
    }
}

rdmat_mp rdmat_mul_sp_mp(rdmat a, rdmat_mp b)
{
    rdmat_mp c = make_rdmat_mp(a.rows, b.cols);
    element_t prod;
    element_init_Zr(prod, pairing);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            element_set0(c.elem[i * b.cols + j]);
            for (int k = 0; k < a.cols; k++)
            {
                element_mul_si(prod, b.elem[k * b.cols + j], a.elem[i * a.cols + k]);
                element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
            }
        }
    }
    element_clear(prod);
    return c;
}
rdmat_mp make_rdmat_mp(int rows, int cols)
{
    rdmat_mp tmp = {.rows = rows, .cols = cols, .elem = (element_t *)malloc(sizeof(element_t) * rows * cols)};
    if (tmp.elem == NULL)
    {
        puts("calloc failed, exiting...");
        exit(-1);
    }
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            element_init_Zr(tmp.elem[i * cols + j], pairing);
        }
    }
    return tmp;
}