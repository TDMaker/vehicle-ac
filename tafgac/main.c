#include <time.h>
#include "algo.h"

int main(void)
{
    clock_t start, finish;
    // A. System Initialization
    // 1) Parameters Set Up
    element_t g, g_1, g_2, a, b, alpha, pk_frag;
    Parameters_Set_Up(&g, &g_1, &g_2, &a, &b, &alpha, &pk_frag);

    // element_printf("The alpha is %B.\n", alpha);
    // 2) Access Policy and Attributes Generation

    /*
     * the policy embedded in M is (0 and 1) or (2 and 3 and 4)
     * so the work set is [0, 1] or [2, 3, 4]
     */
    int attrs_in_ac[] = {0, 1, 2, 3, 4}; // attributes used in Access Policy is 0, 1, 2, 3, 4
    rdmat_f M = gen_M();

    // 3) Application Authentication

    // B. Key Generation and Distribution
    // 1) Key Generation
    rdmat_mp shares = gen_shares_mp(&alpha, M); // shares is ${\lambda_i}$ in the paper
    element_t *K[2];
    Key_Generation(K, shares, &g, attrs_in_ac, shares.rows);
    // 2) Key Distribution

    // C. Data Encryption
    char *ATTRS_IN_CT[] = {
        "Software Developer",
        "Marketing Manager",
        // "Financial Analyst",
        // "Human Resources Specialist",
        // "Data Scientist",
    }; // $\gamma$ in the paper.
    int attr_size_in_ct = sizeof(ATTRS_IN_CT) / sizeof(ATTRS_IN_CT[0]); // attr_size_in_ct is the notation `d` in paper, means there are `d` attribute in cipher text.
    int zeta_I[1024] = {-1};
    init_zeta_I(zeta_I, ATTRS_IN_CT, attr_size_in_ct);
    element_t E_1, E_2, *E_3;
    element_t *msg_encrypted_into = Data_Encryption(&E_1, &E_2, &E_3, ATTRS_IN_CT, &g, &g_1, &g_2, zeta_I, attr_size_in_ct);

    // D. Data Decryption
    // rdmat_mp myshare = pick_rows_mp(attr_size_in_ct, shares, zeta_I);
    element_t *msg_decrypted_from = Data_Decryption(&E_1, &E_2, &E_3, K, M, zeta_I, attr_size_in_ct);

    if (!element_cmp(*msg_encrypted_into, *msg_decrypted_from))
    {
        puts("Decryption succeed.\n");
    }
    else
    {
        puts("Decryption faild!\n");
    }

    // E. Key Update
    // 1) Initialization
    // 2) Membership Change-Based Update
    /**
     * We do not believe that the revocation method proposed in the paper is based on attribute-based encryption, so it is not necessary to embody it here.
     */

    element_clear(g);
    element_clear(g_1);
    element_clear(g_2);
    element_clear(a);
    element_clear(b);
    element_clear(alpha);
    element_clear(pk_frag);
    element_clear(E_1);
    element_clear(E_2);
    element_clear(*msg_encrypted_into);
    element_clear(*msg_decrypted_from);

    for (int i = 0; i < shares.rows; i++)
    {
        element_clear(K[0][i]);
        element_clear(K[1][i]);
    }
    for (int i = 0; i < attr_size_in_ct; i++)
    {
        for (int j = 0; j < attr_size_in_ct; j++)
        {
            element_clear(E_3[i * attr_size_in_ct + j]);
        }
    }

    K[0] = K[1] = E_3 = NULL;

    free_rdmat_f(M);
    free_rdmat_mp(shares);

    return 0;
}
