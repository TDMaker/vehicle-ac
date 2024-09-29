#include "rusc.h"
extern pairing_t pairing;
int main()
{
    // SysInit
    element_t g, h, u, v, w, alpha, pk_frag;
    sys_init(&alpha, &g, &h, &u, &v, &w, &pk_frag);

    // PolicyInit
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";
    element_t M, C, C0, *C1_, *C2_, *C3_, *lambda_;
    policy_init(&C, &C0, &C1_, &C2_, &C3_, &M, &lambda_, &g, &h, &pk_frag, &u, &v, &w, input);

    // KeyDist
    element_t K0, K1, *K2_, *K3_;
    char *my_attr1[] = {"A", "B", "E"};
    key_dist(&K0, &K1, &K2_, &K3_, &alpha, &g, &h, &u, &v, &w, my_attr1, sizeof(my_attr1) / sizeof(my_attr1[0]));

    // Verify
    verify(&C, &C0, &C1_, &C2_, &C3_, &K0, &K1, &K2_, &K3_, &M, my_attr1, sizeof(my_attr1) / sizeof(my_attr1[0]));

    // PolicyMod

    // Clear
    rd_cleanup(&g, &h, &u, &v, &w, &alpha, &pk_frag, &K0, &K1, &K2_, &K3_, &M, &C, &C0, &C1_, &C2_, &C3_, &lambda_);

    return 0;
}