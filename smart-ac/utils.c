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

rdmat_f gaussian_elimination(rdmat a)
{
    int m = a.rows;
    int n = a.cols;

    // 创建必要的矩阵
    rdmat_f aTa = make_rdmat_f(n, n);
    rdmat_f b = make_rdmat_f(n, 1);
    rdmat_f aTb = make_rdmat_f(n, 1);
    b.elem[0] = 1.0f;

    // 计算 aTa 和 aTb
    for (int i = 0; i < n; i++)
    {
        aTb.elem[i] = 0.0f;
        for (int j = 0; j < n; j++)
        {
            aTa.elem[i * n + j] = 0.0f;
            for (int k = 0; k < m; k++)
                aTa.elem[i * n + j] += a.elem[k * n + i] * a.elem[k * n + j];
        }
        for (int k = 0; k < m; k++)
            aTb.elem[i] += a.elem[k * n + i] * b.elem[k];
    }

    // 高斯消元
    for (int i = 0; i < n; i++)
    {
        int maxRow = i;
        float maxAbsPivot = fabs(aTa.elem[i * n + i]);
        for (int j = i + 1; j < n; j++)
        {
            float absVal = fabs(aTa.elem[j * n + i]);
            if (absVal > maxAbsPivot)
            {
                maxAbsPivot = absVal;
                maxRow = j;
            }
        }

        if (maxAbsPivot == 0.0f)
        {
            // 主元为零，无法继续消元
            free_rdmat_f(aTa);
            free_rdmat_f(b);
            free_rdmat_f(aTb);
            return make_rdmat_f(n, 1); // 返回一个空矩阵
        }

        // 交换行
        for (int k = i; k < n; k++)
        {
            float temp = aTa.elem[i * n + k];
            aTa.elem[i * n + k] = aTa.elem[maxRow * n + k];
            aTa.elem[maxRow * n + k] = temp;
        }
        float tempB = aTb.elem[i];
        aTb.elem[i] = aTb.elem[maxRow];
        aTb.elem[maxRow] = tempB;

        // 消元
        for (int j = i + 1; j < n; j++)
        {
            float mult = aTa.elem[j * n + i] / aTa.elem[i * n + i];
            aTb.elem[j] -= mult * aTb.elem[i];
            for (int k = i; k < n; k++)
                aTa.elem[j * n + k] -= mult * aTa.elem[i * n + k];
        }
    }

    // 回代
    rdmat_f c = make_rdmat_f(n, 1);
    for (int i = n - 1; i >= 0; i--)
    {
        c.elem[i] = aTb.elem[i];
        for (int j = i + 1; j < n; j++)
            c.elem[i] -= aTa.elem[i * n + j] * c.elem[j];
        c.elem[i] /= aTa.elem[i * n + i];
    }

    free_rdmat_f(aTa);
    free_rdmat_f(b);
    free_rdmat_f(aTb);
    return c;
}

rdmat_f make_rdmat_f(int rows, int cols)
{
    rdmat_f tmp = {.rows = rows, .cols = cols, .elem = (float *)calloc(sizeof(float), rows * cols)};
    if (tmp.elem == NULL)
    {
        puts("calloc failed, exiting...");
        exit(-1);
    }
    return tmp;
}

rdmat make_rdmat(int rows, int cols)
{
    rdmat tmp = {.rows = rows, .cols = cols, .elem = (int *)calloc(sizeof(int), rows * cols)};
    if (tmp.elem == NULL)
    {
        puts("calloc failed, exiting...");
        exit(-1);
    }
    return tmp;
}

rdmat pick_rows(int count, rdmat a, int *rows)
{
    rdmat c = make_rdmat(count, a.cols);
    int offset = 0;
    int has = 0;

    for (int i = 0; i < count; i++)
    {
        if (rows[i] >= a.rows)
        {
            puts("A row picked exceeds the source matrix!\nexitting...");
            exit(-1);
        }
        has = 0;
        for (int j = 0; j < offset; j++)
        {
            if (memcmp(c.elem + j * c.cols, a.elem + rows[i] * a.cols, a.cols * sizeof(int)) == 0)
            {
                has = 1;
                break;
            }
        }
        if (!has)
        {
            memcpy(c.elem + (offset++) * c.cols, a.elem + rows[i] * a.cols, a.cols * sizeof(int));
        }
    }
    c.rows = offset;
    return c;
}
rdmat_f rdmat_f_mul(rdmat_f a, rdmat_f b)
{
    rdmat_f c = make_rdmat_f(a.rows, b.cols);

    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            for (int k = 0; k < a.cols; k++)
            {
                c.elem[i * c.cols + j] += a.elem[i * a.cols + k] * b.elem[k * b.cols + j];
            }
        }
    }

    return c;
}

void free_rdmat_f(rdmat_f a)
{
    free(a.elem);
    return;
}

void free_rdmat(rdmat a)
{
    free(a.elem);
    return;
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

void rdmat_f_print(const char *name, rdmat_f a)
{
    printf("The matrix **%s** has %d rows and %d cols.\n", name, a.rows, a.cols);
    printf("%s  ", "┌");
    for (int i = 0; i < a.cols - 1; i++)
        printf("\t");
    puts("   ┐");
    for (int i = 0; i < a.rows; i++)
    {
        printf("│");
        for (int j = 0; j < a.cols; j++)
        {

            printf("%.2f", a.elem[i * a.cols + j]);
            if (j < a.cols - 1)
                putchar('\t');
        }
        printf(" │\n");
    }
    printf("%s  ", "└");
    for (int i = 0; i < a.cols - 1; i++)
        putchar('\t');
    puts("   ┘");
    puts("=================================================\n");
    return;
}

void rdmat_print(const char *name, rdmat a)
{
    printf("The matrix **%s** has %d rows and %d cols.\n", name, a.rows, a.cols);
    printf("%s", "┌");
    for (int i = 0; i < a.cols - 1; i++)
        putchar('\t');
    puts("   ┐");
    for (int i = 0; i < a.rows; i++)
    {
        printf("│");
        for (int j = 0; j < a.cols; j++)
        {

            printf("%2d", a.elem[i * a.cols + j]);
            if (j < a.cols - 1)
                putchar('\t');
        }
        printf(" │\n");
    }
    printf("%s", "└");
    for (int i = 0; i < a.cols - 1; i++)
        putchar('\t');
    puts("   ┘");
    puts("=================================================\n");
    return;
}

rdmat transpose(rdmat a)
{
    rdmat c = make_rdmat(a.cols, a.rows);
    for (int i = 0; i < c.rows; i++)
    {
        for (int j = 0; j < c.cols; j++)
        {
            c.elem[i * c.cols + j] = a.elem[j * a.cols + i];
        }
    }
    return c;
}

rdvec make_rdvec()
{
    rdvec tmp = {.length = 1, .data = (int *)calloc(sizeof(int), CAPACITY)};

    if (tmp.data == NULL)
    {
        puts("calloc failed, exiting...");
        exit(-1);
    }
    return tmp;
}

void free_rdvec(rdvec a)
{
    free(a.data);
}

rdvec cpy_rdvec(rdvec a)
{
    rdvec tmp = {.length = a.length, .data = (int *)calloc(sizeof(int), CAPACITY)};
    if (tmp.data == NULL)
    {
        puts("calloc failed, exiting...");
        exit(-1);
    }
    memcpy(tmp.data, a.data, sizeof(int) * CAPACITY);
    return tmp;
}