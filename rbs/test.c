#include <gmp.h>

void my_add(mpz_t _c, mpz_t _a, mpz_t _b)
{
    mpz_add(_c, _a, _b);
}
int main(void)
{
    mpz_t a, b, c;
    mpz_inits(a, b, c, NULL);
    mpz_set_ui(a, 10);
    mpz_set_ui(b, 20);
    my_add(c, a, b);
    gmp_printf("%Zd\n", c);


}