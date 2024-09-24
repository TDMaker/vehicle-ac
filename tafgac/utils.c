#include "utils.h"

extern pairing_t pairing;

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

rdmat_mp rdmat_mul_sp_mp(rdmat_f a, rdmat_mp b)
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

rdmat_mp rdmat_mul_mp_sp(rdmat_mp a, rdmat_f b)
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
                element_mul_si(prod, a.elem[i * a.cols + k], b.elem[k * b.cols + j]);
                element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
            }
        }
    }
    element_clear(prod);
    return c;
}

rdmat_mp gen_shares_mp(element_t *secret, rdmat_f M)
{

    rdmat_mp rho = make_rdmat_mp(M.cols, 1);
    for (int i = 0; i < rho.rows; i++)
    {
        element_random(rho.elem[i]);
    }
    element_set(rho.elem[0], *secret);
    rdmat_mp shares = rdmat_mul_sp_mp(M, rho);
    free_rdmat_mp(rho);
    return shares;
}

rdmat_mp pick_rows_mp(int count, rdmat_mp a, int *rows)
{
    rdmat_mp c = make_rdmat_mp(count, a.cols);

    for (int i = 0; i < count; i++)
    {
        for (int j = 0; j < a.cols; j++)
        {
            element_set(c.elem[i * c.cols + j], a.elem[rows[i] * a.cols + j]);
        }
    }
    return c;
}

rdmat_mp transpose_mp(rdmat_mp a)
{
    rdmat_mp c = make_rdmat_mp(a.cols, a.rows);
    for (int i = 0; i < c.rows; i++)
    {
        for (int j = 0; j < c.cols; j++)
        {
            element_set(c.elem[i * c.cols + j], a.elem[j * a.cols + i]);
        }
    }
    return c;
}