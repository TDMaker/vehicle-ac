#ifndef __RUSC_H__
#define __RUSC_H__
#include "lsss.h"
#include "utils.h"

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

typedef struct
{
    TreeNode **node_;
    int length;
} RHO;

typedef struct
{
    element_t C;
    element_t C0;
    element_t *C1_;
    element_t *C2_;
    element_t *C3_;
    rdmat W;
    RHO rho;
} EV;

typedef struct
{
    rdmat W;
    element_t *lambda;
} IP;

typedef struct
{
    int *s;
    int len_s;
    element_t K0;
    element_t K1;
    element_t *K2_;
    element_t *K3_;
} SK;

typedef enum
{
    LABEL_ADD = 10,
    LABEL_DELETE = 20,
    LABEL_MULTIPLY = 30,
} Label;

typedef enum
{
    STATE_START = 1,
    STATE_DELETE = 2,
    STATE_ADD = 3,
    STATE_MULTIPLY = 4,
    STATE_ADD_ = 5,
    STATE_MULTIPLY_ = 6,
    STATE_REPLACE = 7,
    STATE_UNKNOWN = 8,
} State;

typedef struct
{
    char *attr;
    State state;
} UEV;

void sys_init(PK *, MK *);
void policy_init(EV *, IP *, PK, char *M, char *PP);
void key_dist(SK *, PK, MK, RHO, char **S, int size);
int verify(char *, EV, SK);
void rd_cleanup(PK *, MK *, SK *, EV *, IP *);
void policy_mod(UEV *, IP *, PK, IP, EV *, char *pp);
void evidence_mod(EV *, EV, UEV);

void del_or(TreeNode *node, void *data);
void del_and(TreeNode *node, void *data);
void add_or(TreeNode *sibling, void *data);
#endif /* __RUSC_H__ */