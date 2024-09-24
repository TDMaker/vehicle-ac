#include <string.h>
#include "algo.h"

#define BUFFER_SIZE 1024
char buffer[BUFFER_SIZE];
pairing_t pairing;

char *ATTRIBUTES[] = {
    "Software Developer",
    "Marketing Manager",
    "Financial Analyst",
    "Human Resources Specialist",
    "Data Scientist",
    "Sales Representative",
    "Project Manager",
    "Customer Service Representative",
    "Graphic Designer",
    "Teacher",
};

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
int get_key()
{
    FILE *fp = NULL;
    if ((fp = fopen("/dev/random", "r")) == NULL)
        return -1;
    fread(buffer, 1, BUFFER_SIZE, fp);
    fclose(fp);
    fp = NULL;
    return 0;
}

rdmat_f gen_M()
{
    rdmat_f a_1 = make_rdmat_f(1, 1);
    rdmat_f b_1 = make_rdmat_f(1, 1);
    a_1.elem[0] = 1;
    b_1.elem[0] = 1;

    rdmat_f P_a = get_mand(a_1, b_1);
    rdmat_f P_b = get_mand(P_a, a_1);
    rdmat_f M = get_mor(P_a, P_b);
    free_rdmat_f(a_1);
    free_rdmat_f(b_1);
    free_rdmat_f(P_a);
    free_rdmat_f(P_b);
    return M;
}

void Parameters_Set_Up(element_t *g, element_t *g_1, element_t *g_2, element_t *a, element_t *b, element_t *alpha, element_t *pk_frag)
{
    if (init_pairing() == -1)
    {
        puts("Pairing inits failed!\n");
        exit(-1);
    }

    element_init_G1(*g, pairing);
    element_init_G1(*g_1, pairing);
    element_init_G1(*g_2, pairing);
    element_init_Zr(*a, pairing);
    element_init_Zr(*b, pairing);
    element_init_Zr(*alpha, pairing);
    element_init_GT(*pk_frag, pairing);
    element_random(*g);
    element_random(*a);
    element_random(*b);
    element_pow_zn(*g_1, *g, *a);
    element_pow_zn(*g_2, *g, *b);
    element_mul(*alpha, *a, *b);
    pairing_apply(*pk_frag, *g, *g, pairing);
    element_pow_zn(*pk_frag, *pk_frag, *alpha);
}

void Key_Generation(element_t *K[2], rdmat_mp shares, element_t *g, int *attrs_in_ac, int L)
{

    K[0] = (element_t *)malloc(L * sizeof(element_t));
    K[1] = (element_t *)malloc(L * sizeof(element_t));
    element_t *r_ = (element_t *)malloc(L * sizeof(element_t));

    element_t H_1x, H_2x; // H_1 maps string to Zp, H_2 maps string to G1
    element_init_Zr(H_1x, pairing);
    element_init_G1(H_2x, pairing);

    for (int i = 0; i < L; i++)
    {
        element_init_G1(K[0][i], pairing);
        element_init_G1(K[1][i], pairing);
        element_from_hash(H_1x, ATTRIBUTES[attrs_in_ac[i]], strlen(ATTRIBUTES[attrs_in_ac[i]]));
        element_from_hash(H_2x, ATTRIBUTES[attrs_in_ac[i]], strlen(ATTRIBUTES[attrs_in_ac[i]]));

        element_init_Zr(r_[i], pairing);
        element_random(r_[i]);
        element_mul_zn(H_1x, H_1x, r_[i]);

        element_pow_zn(K[1][i], *g, H_1x);

        element_pow_zn(K[0][i], *g, shares.elem[i]);
        element_pow_zn(H_2x, H_2x, H_1x);
        element_mul(K[0][i], K[0][i], H_2x);
    }
    element_clear(H_1x);
    element_clear(H_2x);
    for (int i = 0; i < L; i++)
    {
        element_clear(r_[i]);
    }
}

void init_zeta_I(int *zeta_I, char **ATTRS_IN_CT, int attr_size_in_ct)
{
    for (int i = 0; i < attr_size_in_ct; i++)
    {
        for (int j = 0; j < sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]); j++)
        {
            if (strcmp(ATTRS_IN_CT[i], ATTRIBUTES[j]) == 0)
            {
                zeta_I[i] = j;
            }
        }
    }
}

element_t *Data_Encryption(element_t *E_1, element_t *E_2, element_t **E_3, char **ATTRS_IN_CT, element_t *g, element_t *g_1, element_t *g_2, int *zeta_I, int attr_size_in_ct)
{
    if (get_key() == -1)
    {
        puts("Key to AES get failed!\n");
        exit(-1);
    }
    printf("There are %d attributes in cipher-text.\n", attr_size_in_ct);

    element_t *message = (element_t *)malloc(sizeof(element_t)); // $M$ in Eq(4)
    element_init_GT(*message, pairing);
    element_t *s_x = (element_t *)malloc(attr_size_in_ct * sizeof(element_t));
    element_t s;
    *E_3 = (element_t *)malloc(attr_size_in_ct * attr_size_in_ct * sizeof(element_t));
    element_init_GT(*message, pairing); // message is the notation $M$ in the paper, which means the plain text (maybe the cipher-text encrypted with AES?).
    element_from_hash(*message, buffer, BUFFER_SIZE);
    element_printf("The message is %B\n", *message);
    element_init_GT(*E_1, pairing);
    element_init_G1(*E_2, pairing);
    element_init_Zr(s, pairing);
    element_set0(s);

    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_init_Zr(s_x[i], pairing);
        element_random(s_x[i]);
        element_add(s, s, s_x[i]);
    }
    element_t hashval;
    element_init_G1(hashval, pairing);
    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_from_hash(hashval, ATTRIBUTES[zeta_I[i]], strlen(ATTRIBUTES[zeta_I[i]]));
        for (int j = 0; j < attr_size_in_ct; j++)
        {
            element_init_G1((*E_3)[i * attr_size_in_ct + j], pairing);
            element_pow_zn((*E_3)[i * attr_size_in_ct + j], hashval, s_x[j]);
        }
    }
    // comment to the above.
    // Each zeta_I's element is the picked attribute's index in the universe $U$
    // e.g. zeta_I[] = {5, 7, 8}, the 5th, 7th, 8th elements in U is picked.
    // The element in E_3 is paired with zeta_I by both index.

    pairing_apply(*E_1, *g_1, *g_2, pairing);
    element_pow_zn(*E_1, *E_1, s);
    element_mul(*E_1, *message, *E_1);
    element_pow_zn(*E_2, *g, s);

    element_clear(s);
    element_clear(hashval);

    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_clear(s_x[i]);
    }
    return message;
}

element_t *Data_Decryption(element_t *E_1, element_t *E_2, element_t **E_3, element_t *K[2], rdmat_f M, int *zeta_I, int attr_size_in_ct)
{
    element_t *D_ = (element_t *)malloc(attr_size_in_ct * sizeof(element_t));
    element_t prod, the_above, the_bottom;
    element_init_GT(the_above, pairing);
    element_init_GT(the_bottom, pairing);
    element_init_G1(prod, pairing);

    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_set1(prod);
        element_init_GT(D_[i], pairing);
        pairing_apply(the_above, *E_2, K[0][zeta_I[i]], pairing);

        for (int j = 0; j < attr_size_in_ct; j++)
        {
            element_mul(prod, prod, (*E_3)[i * attr_size_in_ct + j]);
        }
        pairing_apply(the_bottom, prod, K[1][zeta_I[i]], pairing);
        element_div(D_[i], the_above, the_bottom);
    }
    rdmat_f rows_picked = pick_rows(attr_size_in_ct, M, zeta_I);
    rdmat_f M_AT = transpose(rows_picked);
    rdmat_f lambda_A = gaussian_elimination(M_AT);

    // rdmat_mp myshareT = transpose_mp(myshare);
    // rdmat_mp secret = rdmat_mul_mp_sp(myshareT, lambda_A);
    // element_printf("The secret is %B.\n", secret.elem[0]);

    element_t D_i_w_i, prod_D, lambda_i;
    element_init_GT(D_i_w_i, pairing);
    element_init_GT(prod_D, pairing);
    element_init_Zr(lambda_i, pairing);
    element_set1(prod_D);

    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_set_si(lambda_i, (int)lambda_A.elem[i]);
        element_pow_zn(D_i_w_i, D_[i], lambda_i);
        element_mul(prod_D, prod_D, D_i_w_i);
    }

    element_div(D_i_w_i, *E_1, prod_D);
    element_printf("The message is %B\n", D_i_w_i);
    element_t *result = (element_t *)malloc(sizeof(element_t));
    element_init_GT(*result, pairing);
    element_set(*result, D_i_w_i);
    element_clear(prod);
    element_clear(the_above);
    element_clear(the_bottom);
    free_rdmat_f(rows_picked);
    free_rdmat_f(M_AT);
    free_rdmat_f(lambda_A);
    // free_rdmat_mp(myshare);
    // free_rdmat_mp(myshareT);
    // free_rdmat_mp(secret);
    element_clear(D_i_w_i);
    element_clear(prod_D);
    element_clear(lambda_i);
    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_clear(D_[i]);
    }
    return result;
}
