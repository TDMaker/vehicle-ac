#include "../operator.h"
int main(void)
{
    rdmat_f a_1 = make_rdmat_f(1, 1);
    rdmat_f b_1 = make_rdmat_f(1, 1);
    a_1.elem[0] = 1;
    b_1.elem[0] = 1;

    rdmat_f P_a = get_mand(a_1, b_1);
    rdmat_f P_b = get_mand(P_a, a_1);
    rdmat_f M = get_mor(P_a, P_b);
    rdmat_f_print("M", M);

    float rho_data[] = {2.0f, 3.0f, 1.0f, 4.0f};
    rdmat_f rho = {.elem = rho_data, .cols = 1, .rows = 4};

    rdmat_f cipher_text = rdmat_f_mul(M, rho);
    rdmat_f_print("cipher_text", cipher_text);

    int rows_index[] = {2, 3, 4};

    rdmat_f picked_rows = pick_rows(sizeof(rows_index) / sizeof(rows_index[0]), M, rows_index);
    rdmat_f_print("pick01", picked_rows);

    rdmat_f M_AT = transpose(picked_rows);
    rdmat_f_print("M_AT", M_AT);

    rdmat_f lambda_A = gaussian_elimination(M_AT);

    rdmat_f_print("lambda_A", lambda_A);
    rdmat_f myshare = pick_rows(sizeof(rows_index) / sizeof(rows_index[0]), cipher_text, rows_index);
    rdmat_f myshareT = transpose(myshare);
    rdmat_f_print("myshareT", myshareT);
    rdmat_f secret = rdmat_f_mul(myshareT, lambda_A);
    rdmat_f_print("Secret", secret);

    free_rdmat_f(a_1);
    free_rdmat_f(b_1);
    free_rdmat_f(P_a);
    free_rdmat_f(P_b);
    free_rdmat_f(M);
    free_rdmat_f(cipher_text);
    free_rdmat_f(picked_rows);
    free_rdmat_f(M_AT);
    free_rdmat_f(lambda_A);
    free_rdmat_f(myshare);
    free_rdmat_f(myshareT);
    free_rdmat_f(secret);

    return 0;
}