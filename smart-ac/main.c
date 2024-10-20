#include "rusc.h"
#define COUNT(a) ((sizeof(a)) / (sizeof(a[0])))
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
    char *PP =
    //    "((A)||(B))&&(C)";
     "(Z)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    // char* PP = "E and ((A and B) or (A and C) or (B and C) or (B and D) or (C and D))";
    // char* PP = "((A)&&(B))||((C)||(D))";
    M m;
    EV ev;
    IP ip;
    policy_init(&ev, &ip, pk, &m, PP, false);

    /**
     * KeyDist(PK,MK,S)→{SK}
     */
    char *S[] = {"A", "B", "Z"}; // Don't got any element repeated.
    SK sk;
    char *S2[] = {"C", "D", "W"};
    SK sk2;
    key_dist(&sk, pk, mk, S, COUNT(S));
    key_dist(&sk2, pk, mk, S2, COUNT(S2));
    /**
     * Verify(M,EV,SK)→{0,1}
     */
    verify(m, ev, sk, S, COUNT(S)) == 1 ? puts("Decryption succeed.") : puts("Decryption faild!");
    // verify(m, ev, sk2, S2, COUNT(S2)) == 1 ? puts("Decryption succeed.") : puts("Decryption faild!");

    puts("\n\nmodify@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");

    /**
     * PolicyMod(PK,IP_cur,PP_new)→{UEV,IP_new}
     */
    // char *PP_new = "(E)&&((((A)&&(B))||((C)&&(D)))||((B)&&((C)||(D))))";
    char *PP_new =
    //    "((D)||(B))&&(C)";
    //     "(W)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
         "(W)&&(((C)||(D))||(((E)||(Z))&&((G)||(Y))))";
    // "((C)&&(D))||(((E)||(Z))&&((G)&&(Y)))";
    // "(((12)&&(23))||((46)&&(88)))||(((55)||(11))&&((20)||(89)))";
    UEV uev;
    policy_mod(&uev, pk, &ip, &ev, PP_new);
    /**
     * EvidMod(EV_cur,UEV)→{EV_new}
     */

    evidence_mod(&ev, &uev);

    // result = verify(M, ev_new, sk, S, COUNT(S));
    // result == 1 ? puts("Decryption succeed.") : puts("Decryption faild!");

    verify(m, ev, sk2, S2, COUNT(S2)) == 1 ? puts("S2 Decryption succeed.") : puts("S2 Decryption faild!");

    // Clear
    // rd_cleanup(&pk, &mk, &sk, &ev, &ip);

    return 0;
}
