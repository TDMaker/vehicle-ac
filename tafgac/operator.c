#include "operator.h"

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

void free_rdmat_f(rdmat_f a)
{
    free(a.elem);
    return;
}

void rdmat_f_print(const char *name, rdmat_f a)
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

            printf("%.1f", a.elem[i * a.cols + j]);
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

rdmat_f get_mand(rdmat_f a, rdmat_f b)
{
    rdmat_f c = make_rdmat_f(a.rows + b.rows, a.cols + b.cols);
    for (int i = 0; i < a.rows; ++i)
    {
        int offset_a = 0;
        int offset_c = 0;
        c.elem[i * c.cols + offset_c++] = a.elem[i * a.cols + offset_a];
        c.elem[i * c.cols + offset_c++] = a.elem[i * a.cols + offset_a++];
        for (; offset_a < a.cols; offset_a++, offset_c++)
        {
            c.elem[i * c.cols + offset_c] = a.elem[i * a.cols + offset_a];
        }
    }
    for (int i = 0; i < b.rows; ++i)
    {
        int offset_b = 0;
        int offset_c = 1;
        c.elem[(i + a.rows) * c.cols + offset_c] = b.elem[i * b.cols + offset_b++];
        offset_c += (a.cols - 1);
        for (; offset_b < b.cols; offset_b++, offset_c++)
        {
            c.elem[(i + a.rows) * c.cols + offset_c] = b.elem[i * b.cols + offset_b];
        }
    }
    return c;
}

rdmat_f get_mor(rdmat_f a, rdmat_f b)
{
    rdmat_f c = make_rdmat_f(a.rows + b.rows, a.cols + b.cols - 1);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < a.cols; j++)
        {
            c.elem[(i * c.cols) + j] = a.elem[i * a.cols + j];
        }
    }
    for (int i = 0; i < b.rows; i++)
    {
        int offset_b = 0;
        int offset_c = 0;
        c.elem[(i + a.rows) * c.cols + offset_c] = b.elem[i * b.cols + offset_b];
        offset_b++;
        offset_c += a.cols;
        for (; offset_b < b.cols; offset_b++, offset_c++)
        {
            c.elem[(i + a.rows) * c.cols + offset_c] = b.elem[i * b.cols + offset_b];
        }
    }
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

rdmat_f gaussian_elimination(rdmat_f a)
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

rdmat_f transpose(rdmat_f a)
{
    rdmat_f c = make_rdmat_f(a.cols, a.rows);
    for (int i = 0; i < c.rows; i++)
    {
        for (int j = 0; j < c.cols; j++)
        {
            c.elem[i * c.cols + j] = a.elem[j * a.cols + i];
        }
    }
    return c;
}

rdmat_f pick_rows_22222(int count, rdmat_f a, ...)
{
    rdmat_f c = make_rdmat_f(count, a.cols);
    va_list valist;
    va_start(valist, a);

    for (int i = 0; i < count; i++)
    {
        memcpy(c.elem + i * c.cols, a.elem + va_arg(valist, int) * a.cols, a.cols * sizeof(float));
    }
    va_end(valist);
    return c;
}
rdmat_f pick_rows(int count, rdmat_f a, int *rows)
{
    rdmat_f c = make_rdmat_f(count, a.cols);

    for (int i = 0; i < count; i++)
    {
        memcpy(c.elem + i * c.cols, a.elem + rows[i] * a.cols, a.cols * sizeof(float));
    }
    return c;
}
