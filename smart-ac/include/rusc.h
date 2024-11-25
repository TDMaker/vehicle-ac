#ifndef __RUSC_H__
#define __RUSC_H__
#include "lsss.h"
#include "utils.h"
#include "tree.h"
#include "rd_hashmap.h"
#include "rd_stack.h"
typedef struct
{
    element_t g;
    element_t h;
    element_t u;
    element_t v;
    element_t w;
    element_t frag;
} PK;

typedef element_t MK;
typedef element_t M;

typedef struct
{
    element_t C;
    element_t C0;
    HashMap *CX_;
    rdmat W;
    ptr_list rho;
} EV;

typedef struct
{
    rdmat W;
    HashMap *lambda;
} IP;

typedef struct
{
    element_t K0;
    element_t K1;
    HashMap *KX_;
} SK;

typedef struct
{
    element_t C;
    element_t C0;
    HashMap *CX_;
    rdmat W;
    ptr_list rho;
    HashMap *states;
} UEV;

typedef struct
{
    ptr_list the_remains;
    ptr_list the_deleted;
} RDResult;

void sys_init(PK *, MK *);
void policy_init(EV *, IP *, PK, M *, char *PP, bool is_update);
void key_dist(SK *, PK, MK, char **S, int size);
int verify(M m, EV ev, SK sk, char **s, int my_attr_size);
void policy_mod(UEV *uev, PK pk, IP *ip, EV *ev, char *pp_new);
void evidence_mod(EV *, UEV *);
void rd_cleanup(PK *, MK *, SK *, EV *, IP *);

RDResult get_the_deleted(ptr_list rho1, ptr_list rho2, TreeNode *root1, TreeNode *root2);
bool state_update(HashMap *_map, const char *attribute, Label _label);
element_t *get_this_lambda(HashMap *lambda, TreeNode *node);
HashMap *get_lambda(rdmat a, ptr_list rho, rdmat_mp b);
#endif /* __RUSC_H__ */