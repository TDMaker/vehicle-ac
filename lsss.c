#include <stdio.h>
#include <string.h>
#include <time.h>
#include <pbc/pbc.h>
#include "operator.h"
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];

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

typedef struct
{
    int rows;
    int cols;
    element_t *elem;
} rdmat_mp;

rdmat_mp make_rdmat_mp(int rows, int cols, pairing_t *pairing)
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
            element_init_Zr(tmp.elem[i * cols + j], *pairing);
        }
    }
    return tmp;
}

rdmat_f gen_M()
{
    /*
     * the policy is (A and B) or (C and D and E)
     * so the work set is [0, 1] or [2, 3, 4]
     */
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

void free_rdmat_mp(rdmat_mp target)
{
    for (int i = 0; i < target.rows; i++)
    {
        for (int j = 0; j < target.cols; j++)
        {
            element_clear(target.elem[target.cols * i + j]);
        }
    }
    return;
}

rdmat_mp rdmat_mul_sp_mp(rdmat_f a, rdmat_mp b, pairing_t *pairing)
{
    rdmat_mp c = make_rdmat_mp(a.rows, b.cols, pairing);
    element_t prod;
    element_init_Zr(prod, *pairing);
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

rdmat_mp rdmat_mul_mp_sp(rdmat_mp a, rdmat_f b, pairing_t *pairing)
{

    rdmat_mp c = make_rdmat_mp(a.rows, b.cols, pairing);
    element_t prod;
    element_init_Zr(prod, *pairing);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            element_set0(c.elem[i * b.cols + j]);
            for (int k = 0; k < a.cols; k++)
            {
                element_mul_si(prod, a.elem[i * a.cols + k], b.elem[k * b.cols + j]);
                element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
            }
        }
    }
    element_clear(prod);
    return c;
}

char *map_iarr2_str(int *start, int len)
{
    memset(buffer, 0, BUFFER_SIZE * sizeof(char));
    for (int i = 0; i < len; i++)
    {
        buffer[i] = start[i] + 0x30;
    }
    return buffer;
}

rdmat_mp gen_shares_mp(element_t *secret, rdmat_f M, pairing_t *pairing)
{

    rdmat_mp rho = make_rdmat_mp(M.cols, 1, pairing);
    for (int i = 0; i < rho.rows; i++)
    {
        element_random(rho.elem[i]);
    }
    element_set(rho.elem[0], *secret);
    rdmat_mp shares = rdmat_mul_sp_mp(M, rho, pairing);
    free_rdmat_mp(rho);
    return shares;
}

rdmat_mp pick_rows_mp(int count, rdmat_mp a, int *rows, pairing_t *pairing)
{
    rdmat_mp c = make_rdmat_mp(count, a.cols, pairing);

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < a.cols; j++)
        {
            element_set(c.elem[i * c.cols + j], a.elem[rows[i] * a.cols + j]);
        }
    }
    return c;
}

rdmat_mp transpose_mp(rdmat_mp a, pairing_t *pairing)
{
    rdmat_mp c = make_rdmat_mp(a.cols, a.rows, pairing);
    for (int i = 0; i < c.rows; i++)
    {
        for (int j = 0; j < c.cols; j++)
        {
            element_set(c.elem[i * c.cols + j], a.elem[j * a.cols + i]);
        }
    }
    return c;
}

int main()
{
    clock_t start, finish;
    FILE *fp = NULL;
    if ((fp = fopen("a.param", "r")) == NULL)
    {
        return -1;
    }
    size_t count = fread(buffer, 1, BUFFER_SIZE, fp);
    fclose(fp);
    fp = NULL;

    pairing_t pairing;
    pairing_init_set_buf(pairing, buffer, count);

    // Parameters Set Up
    element_t g, g_1, g_2, a, b, alpha, pk_frag;
    element_init_G1(g, pairing);
    element_init_G1(g_1, pairing);
    element_init_G1(g_2, pairing);
    element_init_Zr(a, pairing);
    element_init_Zr(b, pairing);
    element_init_Zr(alpha, pairing);
    element_init_GT(pk_frag, pairing);
    element_random(g);
    element_random(a);
    element_random(b);
    element_pow_zn(g_1, g, a);
    element_pow_zn(g_2, g, b);
    element_mul(alpha, a, b);
    pairing_apply(pk_frag, g, g, pairing);
    element_pow_zn(pk_frag, pk_frag, alpha);

    element_printf("The alpha is %B.\n", alpha);

    // Generate K

    // attributes used in Access Policy is 0, 1, 2, 3, 4
    int attrs_in_ac[] = {0, 1, 2, 3, 4};
    rdmat_f M = gen_M();
    rdmat_mp shares = gen_shares_mp(&alpha, M, &pairing); // shares is ${\lambda_i}$ in the paper

    int L = shares.rows;

    element_t *K[2], *r_;
    K[0] = (element_t *)malloc(L * sizeof(element_t));
    K[1] = (element_t *)malloc(L * sizeof(element_t));
    r_ = (element_t *)malloc(L * sizeof(element_t));

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

        element_pow_zn(K[1][i], g, H_1x);

        element_pow_zn(K[0][i], g, shares.elem[i]);
        element_pow_zn(H_2x, H_2x, H_1x);
        element_mul(K[0][i], K[0][i], H_2x);
    }

    // Data Encryption
    char *ATTRS_IN_CT[] = {"Financial Analyst", "Human Resources Specialist", "Data Scientist"}; // $\gamma$ in the paper.
    int attr_size_in_ct = sizeof(ATTRS_IN_CT) / sizeof(ATTRS_IN_CT[0]);                          // attr_size_in_ct is the notation `d` in paper, means there are `d` attribute in cipher text.
    printf("There are %d attributes in cipher-text.\n", attr_size_in_ct);

    int zeta_I[BUFFER_SIZE] = {-1};
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

    element_t message; // $M$ in Eq(4)
    element_t E_1;
    element_t E_2;
    element_t *E_3 = (element_t *)malloc(attr_size_in_ct * attr_size_in_ct * sizeof(element_t));
    element_t *s_x = (element_t *)malloc(attr_size_in_ct * sizeof(element_t));
    element_t s;
    element_init_GT(message, pairing); // message is the notation $M$ in the paper, which means the plain text (maybe the cipher-text encrypted with AES?).
    char *plain_text = "8578571111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111";
    element_from_hash(message, plain_text, strlen(plain_text));
    element_printf("The message is %B\n", message);
    element_init_GT(E_1, pairing);
    element_init_G1(E_2, pairing);
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
            element_init_G1(E_3[i * attr_size_in_ct + j], pairing);
            element_pow_zn(E_3[i * attr_size_in_ct + j], hashval, s_x[j]);
        }
    }
    // comment to the above.
    // Each zeta_I's element is the picked attribute's index in the universe $U$
    // e.g. zeta_I[] = {5, 7, 8}, the 5th, 7th, 8th elements in U is picked.
    // The element in E_3 is paired with zeta_I by both index.

    pairing_apply(E_1, g_1, g_2, pairing);
    element_pow_zn(E_1, E_1, s);
    element_mul(E_1, message, E_1);
    element_pow_zn(E_2, g, s);

    // Data Decryption
    element_t *D_ = (element_t *)malloc(attr_size_in_ct * sizeof(element_t));
    element_t prod, the_above, the_bottom;
    element_init_GT(the_above, pairing);
    element_init_GT(the_bottom, pairing);
    element_init_G1(prod, pairing);

    for (int i = 0; i < attr_size_in_ct; i++)
    {
        element_set1(prod);
        element_init_GT(D_[i], pairing);
        pairing_apply(the_above, E_2, K[0][zeta_I[i]], pairing);

        for (int j = 0; j < attr_size_in_ct; j++)
        {
            element_mul(prod, prod, E_3[i * attr_size_in_ct + j]);
        }
        pairing_apply(the_bottom, prod, K[1][zeta_I[i]], pairing);
        element_div(D_[i], the_above, the_bottom);
    }

    rdmat_f pick234 = pick_rows(attr_size_in_ct, M, zeta_I);
    rdmat_f M_AT = transpose(pick234);
    rdmat_f lambda_A = gaussian_elimination(M_AT);
    rdmat_mp myshare = pick_rows_mp(attr_size_in_ct, shares, zeta_I, &pairing);
    rdmat_mp myshareT = transpose_mp(myshare, &pairing);
    rdmat_mp secret = rdmat_mul_mp_sp(myshareT, lambda_A, &pairing);

    element_printf("The secret is %B.\n", secret.elem[0]);

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

    element_div(D_i_w_i, E_1, prod_D);
    element_printf("The message is %B\n", D_i_w_i);

    if (!element_cmp(message, D_i_w_i))
    {
        printf("Decryption succeed.\n");
    }
    else
    {
        printf("Don't match\n");
    }

    element_clear(g);
    element_clear(g_1);
    element_clear(g_2);
    element_clear(a);
    element_clear(b);
    element_clear(alpha);
    element_clear(pk_frag);
    element_clear(H_1x);
    element_clear(H_2x);
    element_clear(message);
    element_clear(E_1);
    element_clear(E_2);
    element_clear(s);
    element_clear(hashval);
    element_clear(prod);
    element_clear(the_above);
    element_clear(the_bottom);
    element_clear(D_i_w_i);
    element_clear(prod_D);
    element_clear(lambda_i);

    for (int i = 0; i < L; i++)
    {
        element_clear(K[0][i]);
        element_clear(K[1][i]);
        element_clear(r_[i]);
    }
    for (int i = 0; i < attr_size_in_ct; i++)
    {
        for (int j = 0; j < attr_size_in_ct; j++)
        {
            element_clear(E_3[i * attr_size_in_ct + j]);
        }
        element_clear(s_x[i]);
        element_clear(D_[i]);
    }
    K[0] = K[1] = E_3 = s_x = r_ = D_ = NULL;

    free_rdmat_f(M);
    free_rdmat_f(pick234);
    free_rdmat_f(M_AT);
    free_rdmat_f(lambda_A);
    free_rdmat_mp(shares);
    free_rdmat_mp(myshare);
    free_rdmat_mp(myshareT);
    free_rdmat_mp(secret);

    return 0;
}
