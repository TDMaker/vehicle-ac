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

void key_dist(element_t **r_, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *alpha, element_t *g, element_t *h, element_t *u, element_t *v, element_t *w, int *S, int vert_S)
{
    *r_ = (element_t *)malloc(sizeof(element_t) * (vert_S + 1));
    *K2_ = (element_t *)malloc(sizeof(element_t) * vert_S);
    *K3_ = (element_t *)malloc(sizeof(element_t) * vert_S);

    element_init_Zr((*r_)[vert_S], pairing);
    element_random((*r_)[vert_S]);
    element_t neg_r, v2neg_r, i_mp;
    element_init_Zr(neg_r, pairing);
    element_neg(neg_r, (*r_)[vert_S]);
    element_init_G1(v2neg_r, pairing);
    element_pow_zn(v2neg_r, *v, neg_r);
    element_init_Zr(i_mp, pairing);

    for (int i = 0; i < vert_S; i++)
    {
        element_init_Zr((*r_)[i], pairing);
        element_random((*r_)[i]);
        element_init_G1((*K2_)[i], pairing);
        element_pow_zn((*K2_)[i], *g, (*r_)[i]);
        element_init_G1((*K3_)[i], pairing);
        element_set_si(i_mp, S[i]);
        element_pow_zn((*K3_)[i], *u, i_mp);
        element_mul((*K3_)[i], (*K3_)[i], *h);
        element_pow_zn((*K3_)[i], (*K3_)[i], (*r_)[i]);
        element_mul((*K3_)[i], (*K3_)[i], v2neg_r);
    }

    element_init_G1(*K0, pairing);
    element_init_G1(*K1, pairing);
    element_t tmp_exp;
    element_init_G1(tmp_exp, pairing);
    element_pow_zn(*K0, *g, *alpha);
    element_pow_zn(tmp_exp, *w, (*r_)[vert_S]);
    element_mul(*K0, *K0, tmp_exp);
    element_pow_zn(*K1, *g, (*r_)[vert_S]);

    element_clear(neg_r);
    element_clear(v2neg_r);
    element_clear(i_mp);
    element_clear(tmp_exp);
}
void policy_init(element_t *C, element_t *C0, element_t **C1_, element_t **C2_, element_t **C3_, element_t *M, element_t **lambda_, element_t *g, element_t *h, element_t *pk_frag, element_t *u, element_t *v, element_t *w, char *input)
{
    TreeNode *root = get_complete_tree(input);
    breadth_first_traversal(root, display);
    get_W_rho(&W, &rho, root);

    element_init_GT(*M, pairing);
    element_random(*M);
    int L = W.rows;
    printf("L is %d\n", L);
    rdmat_mp vec_v = make_rdmat_mp(L, 1);
    for (int i = 0; i < W.cols; i++)
    {
        element_random(vec_v.elem[i]);
    }

    *lambda_ = rdmat_mul_sp_mp(W, vec_v).elem;
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
    *C1_ = (element_t *)malloc(sizeof(element_t) * L);
    *C2_ = (element_t *)malloc(sizeof(element_t) * L);
    *C3_ = (element_t *)malloc(sizeof(element_t) * L);
    element_t tmp1, tmp2;
    element_init_G1(tmp1, pairing);
    element_init_Zr(tmp2, pairing);
    for (int i = 0; i < L; i++)
    {
        element_init_G1((*C1_)[i], pairing);
        element_pow_zn((*C1_)[i], *w, (*lambda_)[i]);
        element_pow_zn(tmp1, *v, t_.elem[i]);
        element_mul((*C1_)[i], (*C1_)[i], tmp1);

        element_init_G1((*C2_)[i], pairing);
        // element_from_hash(tmp2, rho[i], strlen(rho[i]));
        element_set_si(tmp2, i);
        element_pow_zn((*C2_)[i], *u, tmp2);
        element_mul((*C2_)[i], (*C2_)[i], *h);
        element_neg(tmp2, t_.elem[i]);
        element_pow_zn((*C2_)[i], (*C2_)[i], tmp2);

        element_init_G1((*C3_)[i], pairing);
        element_pow_zn((*C3_)[i], *g, t_.elem[i]);
    }
}

void verify(element_t *C, element_t *C0, element_t **C1_, element_t **C2_, element_t **C3_, element_t *K0, element_t *K1, element_t **K2_, element_t **K3_, element_t *M, int *L, int vert_L, element_t* w, element_t** lambda_, element_t** r_, element_t* g)
{
    rdmat rows_picked = pick_rows(vert_L, W, L);
    rdmat_print("rows_picked", rows_picked);

    rdmat W_T = transpose(rows_picked);
    rdmat_print("W_T", W_T);
    rdmat_f omega = gaussian_elimination(W_T);
    rdmat_f_print("omega", omega);

    element_t *B_ = (element_t *)malloc(sizeof(element_t) * vert_L);
    element_t prod, rho_mp, B, omega_mp;
    element_init_GT(prod, pairing);
    element_init_GT(B, pairing);
    element_init_Zr(omega_mp, pairing);
    element_set1(B);


element_t g_w, r_lambda, Bi;
element_init_GT(g_w, pairing);
element_init_Zr(r_lambda, pairing);
element_init_GT(Bi, pairing);
pairing_apply(g_w, *g, *w, pairing);

    for (int i = 0; i < vert_L; i++)
    {
        element_init_GT(B_[i], pairing);
        pairing_apply(B_[i], (*C1_)[L[i]], *K1, pairing);
        pairing_apply(prod, (*C2_)[L[i]], (*K2_)[i], pairing);
        element_mul(B_[i], B_[i], prod);
        pairing_apply(prod, (*C3_)[L[i]], (*K3_)[i], pairing);
        element_mul(B_[i], B_[i], prod);

        
element_mul_zn(r_lambda, (*r_)[vert_L], (*lambda_)[L[i]]);
element_pow_zn(Bi, g_w, r_lambda);

    if (!element_cmp(B_[i], Bi))
    {
        puts("Decryption succeed.\n");
    }
    else
    {
        puts("Decryption faild!\n");
    }

        element_set_si(omega_mp, omega.elem[i]);
        element_pow_zn(B_[i], B_[i], omega_mp);
        element_mul(B, B, B_[i]);
    }
// exit(0);

    element_mul(B, B, *C);
    pairing_apply(prod, *C0, *K0, pairing);
    element_mul(prod, *M, prod);

    if (!element_cmp(B, prod))
    {
        puts("Decryption succeed.\n");
    }
    else
    {
        puts("Decryption faild!\n");
    }
}

// rdmat_mp rdmat_mul_sp_mp(rdmat a, rdmat_mp b)
// {
//     rdmat_mp c = make_rdmat_mp(a.rows, b.cols);
//     element_t prod;
//     element_init_Zr(prod, pairing);
//     for (int i = 0; i < a.rows; i++)
//     {
//         for (int j = 0; j < b.cols; j++)
//         {
//             element_set0(c.elem[i * b.cols + j]);
//             for (int k = 0; k < a.cols; k++)
//             {
//                 element_mul_si(prod, b.elem[k * b.cols + j], a.elem[i * a.cols + k]);
//                 element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
//             }
//         }
//     }
//     element_clear(prod);
//     return c;
// }
// rdmat_mp make_rdmat_mp(int rows, int cols)
// {
//     rdmat_mp tmp = {.rows = rows, .cols = cols, .elem = (element_t *)malloc(sizeof(element_t) * rows * cols)};
//     if (tmp.elem == NULL)
//     {
//         puts("calloc failed, exiting...");
//         exit(-1);
//     }
//     for (int i = 0; i < rows; i++)
//     {
//         for (int j = 0; j < cols; j++)
//         {
//             element_init_Zr(tmp.elem[i * cols + j], pairing);
//         }
//     }
//     return tmp;
// }