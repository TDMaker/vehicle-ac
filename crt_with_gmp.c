#include <gmp.h>
#define G_SIZE 3
// calculate gcd(a, b) and ax + by = gcd(a, b)
void extended_gcd(mpz_t a, mpz_t b, mpz_t x, mpz_t y)
{
    if (mpz_cmp_ui(a, 0) == 0)
    {
        mpz_set_ui(x, 0);
        mpz_set_ui(y, 1);
        return;
    }
    mpz_t x1, y1, tmp;
    mpz_inits(x1, y1, tmp, NULL);
    mpz_mod(tmp, b, a);
    extended_gcd(tmp, a, x1, y1);

    mpz_div(tmp, b, a);
    mpz_submul(y1, tmp, x1);
    mpz_set(x, y1);
    mpz_set(y, x1);
    mpz_clears(x1, y1, tmp, NULL);
    return;
}

void mod_inverse(mpz_t yi, mpz_t a, mpz_t m)
{
    mpz_t x, y;
    mpz_inits(x, y, NULL);
    extended_gcd(a, m, x, y);
    if (mpz_cmp_ui(x, 0) < 0)
    {
        mpz_add(x, x, m);
    }
    mpz_set(yi, x);
    mpz_clears(x, y, NULL);
    return;
}

void chinese_remainder_theorem(mpz_t result, mpz_t *remainders, mpz_t *moduli, int k)
{
    mpz_t M; // Product of all moduli
    mpz_init_set_ui(M, 1);
    for (int i = 0; i < k; i++)
    {
        mpz_mul(M, M, moduli[i]);
    }

    mpz_t Mi, yi, tmp;
    mpz_inits(Mi, yi, tmp, NULL);
    for (int i = 0; i < k; i++)
    {
        mpz_divexact(Mi, M, moduli[i]);
        // Modular inverse of Mi modulo moduli[i]
        mod_inverse(yi, Mi, moduli[i]);
        mpz_mul(tmp, remainders[i], Mi);
        mpz_mul(tmp, tmp, yi);
        mpz_add(result, result, tmp);
    }

    // Ensure the result is within the range [0, M)
    mpz_mod(result, result, M);
    mpz_clears(M, Mi, yi, tmp, NULL);
    return;
}

void go(int *r_remainders, int *r_moduli)
{
    mpz_t remainders[G_SIZE], moduli[G_SIZE], result;
    mpz_init(result);
    for (int i = 0; i < G_SIZE; ++i)
    {
        mpz_init_set_ui(remainders[i], r_remainders[i]);
        mpz_init_set_ui(moduli[i], r_moduli[i]);
    }

    chinese_remainder_theorem(result, remainders, moduli, G_SIZE);
    gmp_printf("The solution is: %Zd\n", result);
    for (int i = 0; i < G_SIZE; ++i)
    {
        mpz_clears(remainders[i], moduli[i], NULL);
    }
    mpz_clear(result);
    return;
}

int main(void)
{
    int r_remainders[G_SIZE] = {2, 3, 2}; // a_i values
    int r_moduli[G_SIZE] = {3, 5, 7};     // n_i values
    go(r_remainders, r_moduli);
    return 0;
}
