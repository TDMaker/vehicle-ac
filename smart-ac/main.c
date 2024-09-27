#include "rusc.h"
extern pairing_t pairing;
int main()
{

    // SysInit
    element_t g, u, h, w, v;
    element_t alpha;
    element_t pk_frag;
    sys_init(&alpha, &g, &h, &u, &v, &w, &pk_frag);

    // KeyDist
    element_t *r_, K0, K1, *K2_, *K3_;
    // int S[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int S[] = {0, 3, 4, 7};    
    key_dist(&r_, &K0, &K1, &K2_, &K3_, &alpha, &g, &h, &u, &v, &w, S, sizeof(S) / sizeof(S[0]));

    // PolicyInit
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";
    element_t M, C, C0, *C1_, *C2_, *C3_, *lambda_;
    policy_init(&C, &C0, &C1_, &C2_, &C3_, &M, &lambda_, &g, &h, &pk_frag, &u, &v, &w, input);
    // Verify
    verify(&C, &C0, &C1_, &C2_, &C3_, &K0, &K1, &K2_, &K3_, &M, S, sizeof(S) / sizeof(S[0]), &w, &lambda_, &r_, &g);
    // for (int i = 0; i < W.rows; i++)
    // {
    //     for (int j = 0; j < W.cols; j++)
    //     {
    //         printf("%2d ", W.elem[i * W.cols + j]);
    //     }
    //     puts("");
    // }

    // PolicyMod

    return 0;
}