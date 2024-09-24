#include <stdio.h>
#include <gmp.h>

// 扩展欧几里得算法，计算gcd(a, b)以及ax + by = gcd(a, b)
void extended_gcd(int a, int b, int *x, int *y)
{
    if (a == 0)
    {
        *x = 0;
        *y = 1;
        return;
    }
    int x1, y1;
    extended_gcd(b % a, a, &x1, &y1);
    *x = y1 - (b / a) * x1;
    *y = x1;
}

// 计算模逆元
int mod_inverse(int a, int m)
{
    int x, y;
    extended_gcd(a, m, &x, &y);
    if (x < 0)
    {
        x += m; // Make sure x is positive
    }
    return x;
}

// 中国剩余定理
int chinese_remainder_theorem(int *remainders, int *moduli, int k)
{
    int M = 1; // Product of all moduli
    for (int i = 0; i < k; i++)
    {
        M *= moduli[i];
    }

    int result = 0;
    for (int i = 0; i < k; i++)
    {
        int Mi = M / moduli[i];              // Partial product
        int yi = mod_inverse(Mi, moduli[i]); // Modular inverse of Mi modulo moduli[i]
        result += remainders[i] * Mi * yi;
    }

    // Ensure the result is within the range [0, M)
    result %= M;

    return result;
}

int main()
{
    int remainders[] = {2, 3, 2}; // a_i values
    int moduli[] = {3, 5, 7};     // n_i values

    int result = chinese_remainder_theorem(remainders, moduli, sizeof remainders / sizeof remainders[0]);
    printf("The solution is: %d\n", result);

    return 0;
}