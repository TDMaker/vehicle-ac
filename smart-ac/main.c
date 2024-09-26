#include <pbc/pbc.h>
#include "rusc.h"
int main()
{

    // SysInit
    element_t g, u, h, w, v;
    element_t alpha;
    element_t pk_frag;
    sys_init(&alpha, &g, &h, &u, &v, &w, &pk_frag);

    // KeyDist
    element_t *r_, K_0, K_1, *K_2, *K_3;
    key_dist(&r_, &K_0, &K_1, &K_2, &K_3, &alpha, &g, &h, &u, &v, &w);

    // PolicyInit
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";
    element_t M, C, C0, *C1, *C2, *C3, *lambda;
    policy_init(&C, &C0, &C1, &C2, &C3, &M, &lambda, &g, &h, &pk_frag, &u, &v, &w, input);

    // for (int i = 0; i < W.rows; i++)
    // {
    //     for (int j = 0; j < W.cols; j++)
    //     {
    //         printf("%2d ", W.elem[i * W.cols + j]);
    //     }
    //     puts("");
    // }

    return 0;
}