#ifndef __RUSC_H__
#define __RUSC_H__
#include "lsss.h"
#include "utils.h"
#include "tree.h"
#include "hashmap.h"

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
    HashMap *CX_;
    HashMap *states;
} UEV;

typedef struct
{
    ptr_list deleted_attributes_connected_by_or;
    ptr_list deleted_attributes_connected_by_and;
    ptr_list added_attributes_connected_by_or;
    ptr_list added_attributes_connected_by_and;
    ptr_list the_remains;
    ptr_list the_universe;
} Result;

Result get_result(ptr_list rho1, ptr_list rho2);

void sys_init(PK *, MK *);
void policy_init(EV *, IP *, PK, M *, char *PP, bool is_update);
void key_dist(SK *, PK, MK, char **S, int size);
int verify(M m, EV ev, SK sk, char **s, int my_attr_size);
void rd_cleanup(PK *, MK *, SK *, EV *, IP *);
void policy_mod(UEV *uev, PK pk, IP *ip, EV *ev, EV *ev_new, char *pp_new);
void evidence_mod(EV *ev_new, EV *ev_cur, UEV *);
bool state_update(HashMap *_map, TreeNode *_node, Label _label); // TODO 换到别的地方

ptr_list get_the_affected(TreeNode *_node);

void del_or(TreeNode *node, void *data);
void del_and(TreeNode *node, void *data);
void add_or(TreeNode *sibling, void *data);

HashMap *get_lambda(rdmat a, ptr_list rho, rdmat_mp b);
#endif /* __RUSC_H__ */