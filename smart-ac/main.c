#include "rusc.h"
int main()
{
    /**
     * SysInit()→{PK,MK}
     * PK=(e,g,h,u,v,w,e(g,g)^\alpha), where e(g,g)^\alpha is pk_frag
     * MK=(\alpha)
     */
    element_t g, h, u, v, w, alpha, pk_frag;
    sys_init(&g, &h, &u, &v, &w, &pk_frag, &alpha);

    /**
     * PolicyInit(PK,M,PP)→{EV,IP}
     * EV=(PP,C,C0,C1,C2,C3)
     * IP=(PP,lambda)
     */
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";
    element_t C, C0, *C1_, *C2_, *C3_, M, *lambda_;
    policy_init(&C, &C0, &C1_, &C2_, &C3_, &M, &lambda_, &g, &h, &u, &v, &w, &pk_frag, input);

    /**
     * KeyDist(PK,MK,S)→{SK}
     * SK=(S,K0,K1,K2,K3)
     */
    char *my_attr1[] = {"A", "B", "E"}; // Decryption fails if any element got repeated.
    element_t K0, K1, *K2_, *K3_;
    key_dist(&K0, &K1, &K2_, &K3_, &alpha, &g, &h, &u, &v, &w, my_attr1, sizeof(my_attr1) / sizeof(my_attr1[0]));

    /**
     * Verify(M,EV,SK)→{0,1}
     */
    verify(&C, &C0, &C1_, &C2_, &C3_, &K0, &K1, &K2_, &K3_, &M, my_attr1, sizeof(my_attr1) / sizeof(my_attr1[0]));

    policy_mod();


    // Clear
    rd_cleanup(&g, &h, &u, &v, &w, &alpha, &pk_frag, &K0, &K1, &K2_, &K3_, &M, &C, &C0, &C1_, &C2_, &C3_, &lambda_);

    return 0;
}
