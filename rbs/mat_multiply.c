void mat_multiply(int row_a, int col_a, int row_b, int col_b, int a[row_a][col_a], int b[row_b][col_b], int c[row_a][col_b])
{

    for (int i = 0; i < row_a; i++)
    {
        for (int j = 0; j < col_b; j++)
        {
            c[i][j] = 0;
            for (int k = 0; k < 2; k++)
                c[i][j] += a[i][k] * b[k][j];
        }
    }
    return;
}