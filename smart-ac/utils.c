#include "utils.h"
extern pairing_t pairing;

ptr_list make_ptr_list(int capacity)
{
    int _capacity = capacity == 0 ? CAPACITY : capacity;
    ptr_list c = {.capacity = CAPACITY, .length = 0, .elem_ = (void **)malloc(sizeof(void *) * _capacity)};
    if (c.elem_ == NULL)
    {
        printf("length %d\n", capacity);
        fprintf(stderr, "ptr_list alloc failed!\n");
        exit(-1);
    }
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

element_t **rdmat_mul_sp_mp(rdmat a, rdmat_mp b)
{
    // rdmat_mp c = make_rdmat_mp(a.rows, b.cols);
    element_t **c = (element_t **)malloc(sizeof(element_t *) * a.rows * b.cols);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            c[i * b.cols + j] = (element_t *)malloc(sizeof(element_t));
            element_init_Zr(*c[i * b.cols + j], pairing);
            element_set0(*c[i * b.cols + j]);
        }
    }
    element_t prod;
    element_init_Zr(prod, pairing);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            for (int k = 0; k < a.cols; k++)
            {
                element_mul_si(prod, b.elem[k * b.cols + j], a.elem[i][k]);
                element_add(*c[i * b.cols + j], *c[i * b.cols + j], prod);
            }
        }
    }
    element_clear(prod);
    return c;
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
//                 element_mul_si(prod, b.elem[k * b.cols + j], a.elem[i][k]);
//                 element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
//             }
//         }
//     }
//     element_clear(prod);
//     return c;
// }
// rdmat_f gaussian_elimination2(rdmat a)
// {
//     int m = a.rows;
//     int n = a.cols;

//     // 创建必要的矩阵
//     rdmat_f aTa = make_rdmat_f(n, n);
//     rdmat_f b = make_rdmat_f(n, 1);
//     rdmat_f aTb = make_rdmat_f(n, 1);
//     b.elem[0] = 1.0f;

//     // 计算 aTa 和 aTb
//     for (int i = 0; i < n; i++)
//     {
//         aTb.elem[i] = 0.0f;
//         for (int j = 0; j < n; j++)
//         {
//             aTa.elem[i * n + j] = 0.0f;
//             for (int k = 0; k < m; k++)
//                 aTa.elem[i * n + j] += a.elem[k][i] * a.elem[k][j];
//         }
//         for (int k = 0; k < m; k++)
//             aTb.elem[i] += a.elem[k][i] * b.elem[k];
//     }

//     // 高斯消元
//     for (int i = 0; i < n; i++)
//     {
//         int maxRow = i;
//         float maxAbsPivot = fabs(aTa.elem[i * n + i]);
//         for (int j = i + 1; j < n; j++)
//         {
//             float absVal = fabs(aTa.elem[j * n + i]);
//             if (absVal > maxAbsPivot)
//             {
//                 maxAbsPivot = absVal;
//                 maxRow = j;
//             }
//         }

//         if (maxAbsPivot == 0.0f)
//         {
//             // 主元为零，无法继续消元
//             free_rdmat_f(aTa);
//             free_rdmat_f(b);
//             free_rdmat_f(aTb);
//             return make_rdmat_f(n, 1); // 返回一个空矩阵
//         }

//         // 交换行
//         for (int k = i; k < n; k++)
//         {
//             float temp = aTa.elem[i * n + k];
//             aTa.elem[i * n + k] = aTa.elem[maxRow * n + k];
//             aTa.elem[maxRow * n + k] = temp;
//         }
//         float tempB = aTb.elem[i];
//         aTb.elem[i] = aTb.elem[maxRow];
//         aTb.elem[maxRow] = tempB;

//         // 消元
//         for (int j = i + 1; j < n; j++)
//         {
//             float mult = aTa.elem[j * n + i] / aTa.elem[i * n + i];
//             aTb.elem[j] -= mult * aTb.elem[i];
//             for (int k = i; k < n; k++)
//                 aTa.elem[j * n + k] -= mult * aTa.elem[i * n + k];
//         }
//     }

//     // 回代
//     rdmat_f c = make_rdmat_f(n, 1);
//     for (int i = n - 1; i >= 0; i--)
//     {
//         c.elem[i] = aTb.elem[i];
//         for (int j = i + 1; j < n; j++)
//             c.elem[i] -= aTa.elem[i * n + j] * c.elem[j];
//         c.elem[i] /= aTa.elem[i * n + i];
//     }

//     free_rdmat_f(aTa);
//     free_rdmat_f(b);
//     free_rdmat_f(aTb);
//     return c;
// }

rdmat_f gaussian_elimination(rdmat augmentedMatrix)
{
    int rank = 0;
    int pivotRow, swapCount = 0;
    double pivotElement;
    rdmat_f solution = make_rdmat_f(augmentedMatrix.cols, 1);
    int cols = augmentedMatrix.cols;
    int rows = augmentedMatrix.rows;

    // 遍历每一列
    for (int col = 0; col < cols - 1 && col < rows; col++)
    {
        pivotElement = fabs(augmentedMatrix.elem[col][col]);
        pivotRow = col;

        // 寻找绝对值最大的元素作为主元
        for (int row = col + 1; row < rows; row++)
        {
            if (fabs(augmentedMatrix.elem[row][col]) > pivotElement)
            {
                pivotElement = fabs(augmentedMatrix.elem[row][col]);
                pivotRow = row;
            }
        }

        // 如果主元为0，则无解
        if (pivotElement == 0)
        {
            continue;
        }

        // 交换行
        if (pivotRow != col)
        {
            for (int k = col; k < cols; k++)
            {
                double temp = augmentedMatrix.elem[col][k];
                augmentedMatrix.elem[col][k] = augmentedMatrix.elem[pivotRow][k];
                augmentedMatrix.elem[pivotRow][k] = temp;
            }
            swapCount++;
        }

        // 将主元下方的元素变为0
        for (int row = col + 1; row < rows; row++)
        {
            double factor = augmentedMatrix.elem[row][col] / augmentedMatrix.elem[col][col];
            for (int k = col; k < cols; k++)
            {
                augmentedMatrix.elem[row][k] -= factor * augmentedMatrix.elem[col][k];
            }
        }
        rank++;
    }

    // 检查是否存在矛盾方程
    for (int row = rank; row < rows; row++)
    {
        double sum = 0.0;
        for (int col = 0; col < cols - 1; col++)
        {
            sum += augmentedMatrix.elem[row][col] * 1.0;
        }
        if (fabs(sum - augmentedMatrix.elem[row][cols - 1]) > 1e-10)
        {
            puts("There are contradictory equations");
            exit(-1); // 存在矛盾方程
        }
    }

    // 如果存在多余自由变量，则随机设置这些变量
    if (rank < cols - 1)
    {
        srand(time(NULL));
        for (int i = rank; i < cols - 1; i++)
        {
            solution.elem[i] = (float)(rand() % 100) / 100.0;
        }
    }

    // 回代求解
    for (int i = rank - 1; i >= 0; i--)
    {
        float sum = 0.0f;
        for (int j = i + 1; j < cols - 1; j++)
        {
            sum += augmentedMatrix.elem[i][j] * solution.elem[j];
        }
        solution.elem[i] = (augmentedMatrix.elem[i][cols - 1] - sum) / augmentedMatrix.elem[i][i];
    }

    return solution;
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
    rdmat tmp = {.rows = rows, .cols = cols, .elem = (int **)malloc(sizeof(int *) * rows * cols)};
    for (int i = 0; i < rows; i++)
    {
        tmp.elem[i] = (int *)calloc(sizeof(int), cols);
        if (tmp.elem[i] == NULL)
        {
            puts("calloc failed, exiting...");
            exit(-1);
        }
    }
    if (tmp.elem == NULL)
    {
        puts("malloc failed, exiting...");
        exit(-1);
    }
    return tmp;
}

// rdmat pick_rows_bak(int count, rdmat a, int *rows)
// {
//     rdmat c = make_rdmat(count, a.cols);
//     int offset = 0;
//     int has = 0;

//     for (int i = 0; i < count; i++)
//     {
//         if (rows[i] >= a.rows)
//         {
//             puts("A row picked exceeds the source matrix!\nexitting...");
//             exit(-1);
//         }
//         has = 0;
//         for (int j = 0; j < offset; j++)
//         {
//             if (memcmp(c.elem + j * c.cols, a.elem + rows[i] * a.cols, a.cols * sizeof(int)) == 0)
//             {
//                 has = 1;
//                 break;
//             }
//         }
//         if (!has)
//         {
//             memcpy(c.elem + (offset++) * c.cols, a.elem + rows[i] * a.cols, a.cols * sizeof(int));
//         }
//     }
//     c.rows = offset;
//     return c;
// }

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
        // for (int j = 0; j < offset; j++)
        // {
        //     if (memcmp(c.elem[j], a.elem[rows[i]], a.cols * sizeof(int)) == 0)
        //     {
        //         has = 1;
        //         break;
        //     }
        // }
        if (!has)
        {
            memcpy(c.elem[offset++], a.elem[rows[i]], a.cols * sizeof(int));
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

rdmat get_arged_mat(rdmat a)
{
    rdmat c = make_rdmat(a.rows, a.cols + 1);
    for (int i = 0; i < c.rows; i++)
    {
        for (int j = 0; j < a.cols; j++)
        {
            c.elem[i][j] = a.elem[i][j];
        }
        c.elem[i][a.cols] = 0;
    }
    c.elem[0][a.cols] = 1;
    return c;
}

void free_rdmat_f(rdmat_f a)
{
    free(a.elem);
    return;
}

// void free_rdmat(rdmat a)
// {
//     free(a.elem);
//     return;
// }

void free_rdmat(rdmat a)
{
    for (int i = 0; i < a.rows; i++)
    {
        free(a.elem[i]);
    }
    free(a.elem);
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
    puts("    ┐");
    for (int i = 0; i < a.rows; i++)
    {
        printf("│");
        for (int j = 0; j < a.cols; j++)
        {

            printf("%5.2f", a.elem[i * a.cols + j]);
            if (j < a.cols - 1)
                putchar('\t');
        }
        printf(" │\n");
    }
    printf("%s  ", "└");
    for (int i = 0; i < a.cols - 1; i++)
        putchar('\t');
    puts("    ┘");
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

            printf("%2d", a.elem[i][j]);
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
            c.elem[i][j] = a.elem[j][i];
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