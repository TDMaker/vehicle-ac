#include "rusc.h"
#define COUNT(a) ((sizeof(a)) / (sizeof(a[0])))
int main()
{
    /******************************* SysInit()→{PK,MK} *******************************************************************/
    PK pk;
    MK mk;
    sys_init(&pk, &mk);

    /******************************* PolicyInit(PK,M,PP)→{EV,IP} **********************************************************/
    char *PP =
        "(Z)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    // char* PP = "E and ((A and B) or (A and C) or (B and C) or (B and D) or (C and D))";
    M m;
    EV ev;
    IP ip;
    policy_init(&ev, &ip, pk, &m, PP, false);

    /******************************* KeyDist(PK,MK,S)→{SK}*******************************************************************/
    char *S[] = {"A", "B", "Z"}; // Don't got any element repeated.
    SK sk;
    key_dist(&sk, pk, mk, S, COUNT(S));

    char *S2[] = {"E", "G", "Z"};
    SK sk2;
    key_dist(&sk2, pk, mk, S2, COUNT(S2));

    char *S3[] = {"Y", "G", "Z"};
    SK sk3;
    key_dist(&sk3, pk, mk, S3, COUNT(S3));

    /******************************* Verify(M,EV,SK)→{0,1} *******************************************************************/
    verify(m, ev, sk, S, COUNT(S)) == 1 ? puts("SK1 Decryption succeed.") : puts("SK1 Decryption faild!");
    verify(m, ev, sk2, S2, COUNT(S2)) == 1 ? puts("SK2 Decryption succeed.") : puts("SK2 Decryption faild!");
    verify(m, ev, sk3, S3, COUNT(S3)) == 1 ? puts("SK3 Decryption succeed.") : puts("SK3 Decryption faild!");

    puts("\n\nmodify@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
    /******************************* PolicyMod(PK,IP_cur,PP_new)→{UEV,IP_new}***********************************************/
    char *PP_new =
        "(Z)&&((((A)&&(B))||((C)&&(D)))||(((Y)||(F))&&((G)||(H))))";
    UEV uev;
    policy_mod(&uev, pk, &ip, &ev, PP_new);

    /******************************* EvidMod(EV_cur,UEV)→{EV_new} **********************************************************/

    evidence_mod(&ev, &uev);

    verify(m, ev, sk3, S3, COUNT(S3)) == 1 ? puts("S3 Decryption succeed.") : puts("S3 Decryption faild!");

    // Clear
    // rd_cleanup(&pk, &mk, &sk, &ev, &ip);

    return 0;
}
