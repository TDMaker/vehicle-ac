#include "rusc.h"
int main()
{
    /**
     * SysInit()→{PK,MK}
     */
    PK pk;
    MK mk;
    sys_init(&pk, &mk);

    /**
     * PolicyInit(PK,M,PP)→{EV,IP}
     */
    // char *PP = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";
    char *PP = "(Z)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    // char* PP = "E and ((A and B) or (A and C) or (B and C) or (B and D) or (C and D))";
    // char* PP = "((A)&&(B))||((C)||(D))";
    char *M = "1234567890";
    EV ev;
    IP ip;
    policy_init(&ev, &ip, pk, M, PP, false);

    /**
     * KeyDist(PK,MK,S)→{SK}
     */
    char *S[] = {"A", "B", "Z"}; // Decryption fails if any element got repeated.
    SK sk;
    key_dist(&sk, pk, mk, ev.rho, S, sizeof(S) / sizeof(S[0]));

    /**
     * Verify(M,EV,SK)→{0,1}
     */
    if (verify(M, ev, sk, S, sizeof(S) / sizeof(S[0])))
    {
        puts("Decryption succeed.\n");
    }
    else
    {
        puts("Decryption faild!\n");
    }

    /**
     * PolicyMod(PK,IP_cur,PP_new)→{UEV,IP_new}
     */
    // char *PP_new = "(E)&&((((A)&&(B))||((C)&&(D)))||((B)&&((C)||(D))))";
    char *PP_new = \
    "(W)&&(((C)&&(D))||(((E)||(Z))&&((G)&&(Y))))";
    // "((C)&&(D))||(((E)||(Z))&&((G)&&(Y)))";
    // "(((12)&&(23))||((46)&&(88)))||(((55)||(11))&&((20)||(89)))";
    UEV uev;
    EV ev_new;
    // policy_mod(&uev, pk, &ip, &ev, &ev_new, PP_new);

    /**
     * EvidMod(EV_cur,UEV)→{EV_new}
     */
    // evidence_mod(&ev_new, &ev, &uev);

    // Clear
    // rd_cleanup(&pk, &mk, &sk, &ev, &ip);

    return 0;
}
