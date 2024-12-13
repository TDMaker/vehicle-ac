#include <rusc.h>
#define STRINGIFY(x) #x
#define TRANSFORM(expr) "(Z)&&((Y)&&("STRINGIFY(expr)"))"
int main()
{
    /******************************* SysInit()→{PK,MK} *******************************************************************/
    PK pk;
    MK mk;
    sys_init(&pk, &mk);

    /******************************* PolicyInit(PK,M,PP)→{EV,IP} **********************************************************/
    char *PP = TRANSFORM((K)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H)))));
               // "(Z)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    //    TRANSFORM((K)&&(A));
    // "(Z)&&((A)&&(C))";
    // "(Z)&&((Y)&&((A)&&(C)))";
    puts(PP);
    // char* PP = "E and ((A and B) or (A and C) or (B and C) or (B and D) or (C and D))";
    M m;
    EV ev;
    IP ip;
    policy_init(&ev, &ip, pk, &m, PP);

    /******************************* KeyDist(PK,MK,K)→{SK}*******************************************************************/
    char *S[] = {"Z", "Y", "K", "A", "F", "G"}; // Don't got any element repeated.
    SK sk;
    key_dist(&sk, pk, mk, S, COUNT(S));

    char *S2[] = {"Z", "Y", "K", "B", "A", "C"};
    SK sk2;
    key_dist(&sk2, pk, mk, S2, COUNT(S2));

    char *S3[] = {"Z", "Y", "K", "E", "G"};
    SK sk3;
    key_dist(&sk3, pk, mk, S3, COUNT(S3));

    char *S4[] = {"Z", "Y", "B", "E"};
    SK sk4;
    key_dist(&sk4, pk, mk, S4, COUNT(S4));

    /******************************* Verify(M,EV,SK)→{0,1} *******************************************************************/
    verify(m, ev, sk, S, COUNT(S)) == 1 ? puts("SK1 Decryption succeed.") : puts("SK1 Decryption faild!");
    verify(m, ev, sk2, S2, COUNT(S2)) == 1 ? puts("SK2 Decryption succeed.") : puts("SK2 Decryption faild!");
    verify(m, ev, sk3, S3, COUNT(S3)) == 1 ? puts("SK3 Decryption succeed.") : puts("SK3 Decryption faild!");
    verify(m, ev, sk4, S4, COUNT(S4)) == 1 ? puts("SK4 Decryption succeed.") : puts("SK4 Decryption faild!");

    puts("\n\nmodify@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n\n");
    /******************************* PolicyMod(PK,IP_cur,PP_new)→{UEV,IP_new}***********************************************/
    char *PP_new = TRANSFORM((K)&&((((A)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H)))));
    //"(Z)&&((Y)&&(A))";
    // "(Z)&&((((Y)&&(B))||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    // "(Z)&&(((B)||((C)&&(D)))||(((E)||(F))&&((G)||(H))))";
    // "(Z)&&(((C)&&(D))||(((E)||(F))&&((G)||(H))))";
    // "(Z)&&(((D)||(((E)||(F))&&((G)||(H))))";
    // "(Z)&&(((E)||(F))&&((G)||(H)))";
    // "(Z)&&((F)&&((G)||(H)))";
    // "(Z)&&((G)||(H))";
    // "(Z)&&(H)";
    // "(A)";
    // "(Z)&&((A)&&((C)&&(H)))";
    // "(Z)||(B)";
    puts(PP_new);
    UEV uev;
    policy_mod(&uev, pk, &ip, &ev, PP_new);

    /******************************* EvidMod(EV_cur,UEV)→{EV_new} **********************************************************/

    evidence_mod(&uev, &ev);
    verify(m, ev, sk, S, COUNT(S2)) == 1 ? puts("SK1 Decryption succeed.") : puts("SK1 Decryption faild!");
    verify(m, ev, sk2, S2, COUNT(S2)) == 1 ? puts("SK2 Decryption succeed.") : puts("SK2 Decryption faild!");
    verify(m, ev, sk3, S3, COUNT(S3)) == 1 ? puts("SK3 Decryption succeed.") : puts("SK3 Decryption faild!");
    verify(m, ev, sk4, S4, COUNT(S4)) == 1 ? puts("SK4 Decryption succeed.") : puts("SK4 Decryption faild!");

    // Clear
    // rd_cleanup(&pk, &mk, &sk, &ev, &ip);

    return 0;
}

/**
 * 目前的状态：
 * 代码无内存bug，加解密没问题，但策略修改部分需要改
 * 论文中，策略更改时，服务端依旧需要重新运行策略初始化，这不经济
 * 要改成按需重新生成组件
 * 然后通过什么方式发给终端，然后终端通过什么计算得到新的evidence
 * 总之从evidence modify后就需要重新设计了。
 */