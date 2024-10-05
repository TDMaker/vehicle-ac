#include "rusc.h"
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
pairing_t pairing; // Pairing that should be in PK is placed in global scope so that it can be linked correctly by other compiled modules.
TreeNode *root;
State transition(State state, Label label)
{
    switch (state + label)
    {
    case STATE_START + LABEL_ADD:
        return STATE_ADD;
    case STATE_START + LABEL_DELETE:
        return STATE_DELETE;
    case STATE_START + LABEL_MULTIPLY:
        return STATE_MULTIPLY;
    case STATE_ADD + LABEL_MULTIPLY:
        return STATE_MULTIPLY;
    case STATE_ADD + LABEL_DELETE:
        return STATE_DELETE;
    case STATE_MULTIPLY + LABEL_MULTIPLY:
        return STATE_MULTIPLY;
    case STATE_MULTIPLY + LABEL_DELETE:
        return STATE_DELETE;
    case STATE_DELETE + LABEL_ADD:
        return STATE_ADD_;
    case STATE_ADD_ + LABEL_DELETE:
        return STATE_DELETE;
    case STATE_ADD_ + LABEL_MULTIPLY:
        return STATE_MULTIPLY_;
    case STATE_MULTIPLY_ + LABEL_MULTIPLY:
        return STATE_MULTIPLY_;
    case STATE_MULTIPLY_ + LABEL_DELETE:
        return STATE_DELETE;
    default:
        return STATE_UNKNOWN;
    }
}

int init_pairing()
{
    FILE *fp = NULL;
    if ((fp = fopen("a.param", "r")) == NULL)
        return -1;
    size_t count = fread(buffer, 1, BUFFER_SIZE, fp);
    fclose(fp);
    fp = NULL;
    pairing_init_set_buf(pairing, buffer, count);
    return 0;
}

void sys_init(PK *pk, MK *mk)
{
    if (init_pairing() == -1)
    {
        puts("Pairing inits failed!\n");
        exit(-1);
    }
    element_init_G1(pk->g, pairing);
    element_init_G1(pk->h, pairing);
    element_init_G1(pk->u, pairing);
    element_init_G1(pk->v, pairing);
    element_init_G1(pk->w, pairing);
    element_init_GT(pk->frag, pairing); // pk->frag = e(g,g)^\alpha
    element_init_Zr(*mk, pairing);

    element_random(pk->g);
    element_random(pk->h);
    element_random(pk->u);
    element_random(pk->v);
    element_random(pk->w);
    element_random(*mk);
    pairing_apply(pk->frag, pk->g, pk->g, pairing);
    element_pow_zn(pk->frag, pk->frag, *mk);
}
void policy_init(EV *ev, IP *ip, PK pk, char *_m, char *pp)
{
    root = get_complete_tree(pp);
    breadth_first_traversal(root, display, NULL);
    get_W_rho(&(ev->W), &(ev->rho.node_), root);
    // rdmat_print("W", ev->W);
    ev->rho.length = ev->W.rows;

    element_t m;
    element_init_GT(m, pairing);
    element_from_hash(m, _m, strlen(_m));
    int L = ev->W.rows;
    printf("L is %d\n", L);
    rdmat_mp vec_v = make_rdmat_mp(L, 1);
    for (int i = 0; i < ev->W.cols; i++)
    {
        element_random(vec_v.elem[i]);
    }

    ip->lambda = rdmat_mul_sp_mp(ev->W, vec_v).elem;
    ip->W = ev->W;
    rdmat_mp t_ = make_rdmat_mp(1, L);
    for (int i = 0; i < L; i++)
    {
        element_random(t_.elem[i]);
    }

    element_t tmp1, tmp2, tmp3;
    element_init_G1(tmp1, pairing);
    element_init_Zr(tmp2, pairing);
    element_init_GT(tmp3, pairing);

    element_init_GT(ev->C, pairing);
    element_pow_zn(tmp3, pk.frag, vec_v.elem[0]);
    element_mul(ev->C, m, tmp3);

    element_init_G1(ev->C0, pairing);
    element_pow_zn(ev->C0, pk.g, vec_v.elem[0]);
    ev->C1_ = (element_t *)malloc(sizeof(element_t) * L);
    ev->C2_ = (element_t *)malloc(sizeof(element_t) * L);
    ev->C3_ = (element_t *)malloc(sizeof(element_t) * L);
    for (int i = 0; i < L; i++)
    {
        element_init_G1(ev->C1_[i], pairing);
        element_pow_zn(ev->C1_[i], pk.w, ip->lambda[i]);
        element_pow_zn(tmp1, pk.v, t_.elem[i]);
        element_mul(ev->C1_[i], ev->C1_[i], tmp1);

        element_init_G1(ev->C2_[i], pairing);
        element_set_si(tmp2, i);
        element_pow_zn(ev->C2_[i], pk.u, tmp2);
        element_mul(ev->C2_[i], ev->C2_[i], pk.h);
        element_neg(tmp2, t_.elem[i]);
        element_pow_zn(ev->C2_[i], ev->C2_[i], tmp2);

        element_init_G1(ev->C3_[i], pairing);
        element_pow_zn(ev->C3_[i], pk.g, t_.elem[i]);
    }

    element_clear(tmp1);
    element_clear(tmp2);
    element_clear(tmp3);
    element_clear(m);
    free_rdmat_mp(vec_v);
    free_rdmat_mp(t_);
    // rd_free_tree(root);
}
void key_dist(SK *sk, PK pk, MK mk, RHO rho, char **s, int my_attr_size)
{
    int S[1024] = {-1};
    sk->len_s = 0;
    for (int i = 0; i < rho.length; i++)
    {
        for (int j = 0; j < my_attr_size; j++)
        {
            if (strcmp(rho.node_[i]->value, s[j]) == 0)
            {
                S[sk->len_s++] = i;
            }
        }
    }
    sk->s = (int *)malloc(sizeof(int) * sk->len_s);
    memcpy(sk->s, S, sizeof(int) * sk->len_s);

    element_t *r_ = (element_t *)malloc(sizeof(element_t) * (sk->len_s + 1));
    sk->K2_ = (element_t *)malloc(sizeof(element_t) * sk->len_s);
    sk->K3_ = (element_t *)malloc(sizeof(element_t) * sk->len_s);

    element_init_Zr(r_[sk->len_s], pairing);
    element_random(r_[sk->len_s]);
    element_t neg_r, v2neg_r, i_mp;
    element_init_Zr(neg_r, pairing);
    element_neg(neg_r, r_[sk->len_s]);
    element_init_G1(v2neg_r, pairing);
    element_pow_zn(v2neg_r, pk.v, neg_r);
    element_init_Zr(i_mp, pairing);

    for (int i = 0; i < sk->len_s; i++)
    {
        element_init_Zr(r_[i], pairing);
        element_random(r_[i]);
        element_init_G1(sk->K2_[i], pairing);
        element_pow_zn(sk->K2_[i], pk.g, r_[i]);
        element_init_G1(sk->K3_[i], pairing);
        element_set_si(i_mp, S[i]);
        element_pow_zn(sk->K3_[i], pk.u, i_mp);
        element_mul(sk->K3_[i], sk->K3_[i], pk.h);
        element_pow_zn(sk->K3_[i], sk->K3_[i], r_[i]);
        element_mul(sk->K3_[i], sk->K3_[i], v2neg_r);
    }

    element_init_G1(sk->K0, pairing);
    element_init_G1(sk->K1, pairing);
    element_t tmp_exp;
    element_init_G1(tmp_exp, pairing);
    element_pow_zn(sk->K0, pk.g, mk);
    element_pow_zn(tmp_exp, pk.w, r_[sk->len_s]);
    element_mul(sk->K0, sk->K0, tmp_exp);
    element_pow_zn(sk->K1, pk.g, r_[sk->len_s]);

    element_clear(neg_r);
    element_clear(v2neg_r);
    element_clear(i_mp);
    element_clear(tmp_exp);
    for (int i = 0; i < sk->len_s + 1; i++)
    {
        element_clear(r_[i]);
    }
    free(r_);
}
int verify(char *_m, EV ev, SK sk)
{
    element_t m;
    element_init_GT(m, pairing);
    element_from_hash(m, _m, strlen(_m));

    rdmat rows_picked = pick_rows(sk.len_s, ev.W, sk.s);
    rdmat_print("rows_picked", rows_picked);

    rdmat W_T = transpose(rows_picked);
    rdmat_print("W_T", W_T);
    rdmat_f omega = gaussian_elimination(W_T);
    rdmat_f_print("omega", omega);

    element_t B_i, prod, B, omega_mp;
    element_init_GT(B_i, pairing);
    element_init_GT(prod, pairing);
    element_init_GT(B, pairing);
    element_init_Zr(omega_mp, pairing);
    element_set1(B);

    for (int i = 0; i < sk.len_s; i++)
    {
        pairing_apply(B_i, ev.C1_[sk.s[i]], sk.K1, pairing);
        pairing_apply(prod, ev.C2_[sk.s[i]], sk.K2_[i], pairing);
        element_mul(B_i, B_i, prod);
        pairing_apply(prod, ev.C3_[sk.s[i]], sk.K3_[i], pairing);
        element_mul(B_i, B_i, prod);

        element_set_si(omega_mp, i >= omega.rows ? 0 : omega.elem[i]);
        element_pow_zn(B_i, B_i, omega_mp);
        element_mul(B, B, B_i);
    }

    element_mul(B, B, ev.C);
    pairing_apply(prod, ev.C0, sk.K0, pairing);
    element_mul(prod, m, prod);

    int result = !element_cmp(B, prod);

    element_clear(B_i);
    element_clear(prod);
    element_clear(B);
    element_clear(omega_mp);
    element_clear(m);
    free_rdmat(rows_picked);
    free_rdmat(W_T);
    free_rdmat_f(omega);
    return result;
}
void rd_cleanup(PK *pk, MK *mk, SK *sk, EV *ev, IP *ip)
{
    element_clear(pk->g);
    element_clear(pk->h);
    element_clear(pk->u);
    element_clear(pk->v);
    element_clear(pk->w);
    element_clear(*mk);
    element_clear(pk->frag);
    element_clear(sk->K0);
    element_clear(sk->K1);
    element_clear(ev->C);
    element_clear(ev->C0);
    free_rdmat(ev->W);
    // for (int i = 0; i < ev->rho.len; i++)
    // {
    //     free(ev->rho.node);
    // }
    free(ev->rho.node_);
    for (int i = 0; i < sizeof(sk->K2_) / sizeof(sk->K2_[0]); i++)
    {
        element_clear(sk->K2_[i]);
        element_clear(sk->K3_[i]);
    }
    free(sk->s);
    free(sk->K2_);
    free(sk->K3_);
    for (int i = 0; i < sizeof(ev->C1_) / sizeof(ev->C1_[0]); i++)
    {
        element_clear(ev->C1_[i]);
        element_clear(ev->C2_[i]);
        element_clear(ev->C3_[i]);
        element_clear(ip->lambda[i]);
    }
    free(ev->C1_);
    free(ev->C2_);
    free(ev->C3_);
    free(ip->lambda);

    pairing_clear(pairing);
}

void policy_mod(UEV *uev, IP *ip_new, PK pk, IP ip, char *pp_new)
{
    State new = transition(STATE_START, LABEL_ADD);
    printf("%d\n", new);
    // add_or(root, "F");
    // TODO:
    // get_W_rho();
    // breadth_first_traversal(root, display, NULL);
}

void evidence_mod(EV *ev_new, EV ev, UEV uev)
{
}

void del_or(TreeNode *node, void *data)
{
    char *value = (char *)data;
    if (strcmp(node->value, value) == 0 && strcmp(node->parent->value, "||") == 0)
    {
        TreeNode *silbling = node->parent->left == node ? node->parent->right : node->parent->left;
        strcpy(node->parent->value, silbling->value);
        silbling->parent->left = silbling->left;
        silbling->parent->right = silbling->right;
        free_rdvec(silbling->vec);
        free(silbling);
        free_rdvec(node->vec);
        free(node);
    }
}

void del_and(TreeNode *node, void *data)
{
    char *value = (char *)data;
    if (strcmp(node->value, value) == 0 && strcmp(node->parent->value, "&&") == 0)
    {
        TreeNode *silbling = node->parent->left == node ? node->parent->right : node->parent->left;
        for (int i = 0; i < silbling->vec.length; i++)
        {
            silbling->vec.data[i] += node->vec.data[i];
        }
        strcpy(silbling->parent->value, silbling->value);
        free_rdvec(silbling->parent->vec);
        silbling->parent->vec = silbling->vec;
        silbling->parent->left = silbling->left;
        silbling->parent->right = silbling->right;
        free(silbling);
        free_rdvec(node->vec);
        free(node);
    }
}

void add_or(TreeNode *silbling, void *data) // 我要选择跟谁做兄弟
{
    TreeNode *new_silbling = (TreeNode *)malloc(sizeof(TreeNode));
    memcpy(new_silbling, silbling, sizeof(TreeNode));
    silbling->left = new_silbling;
    new_silbling->parent = silbling;
    new_silbling->vec = cpy_rdvec(silbling->vec);
    strcpy(silbling->value, "||");
    silbling->right = (TreeNode *)malloc(sizeof(TreeNode));
    silbling->right->parent = silbling;
    strcpy(silbling->right->value, (char *)data);
    silbling->right->left = NULL;
    silbling->right->right = NULL;
    silbling->right->vec = cpy_rdvec(silbling->vec);
    print_node(silbling);
}

void add_and(TreeNode *silbling, void *data)
{
    TreeNode *new_silbling = (TreeNode *)malloc(sizeof(TreeNode));
    memcpy(new_silbling, silbling, sizeof(TreeNode));
    silbling->left = new_silbling;
    new_silbling->parent = silbling;
    new_silbling->vec = cpy_rdvec(silbling->vec);
    new_silbling->vec.data[new_silbling->vec.length] = 1;
    new_silbling->vec.length++;
    strcpy(silbling->value, "&&");
    silbling->right = (TreeNode *)malloc(sizeof(TreeNode));
    silbling->right->parent = silbling;
    strcpy(silbling->right->value, "D");
    silbling->right->left = NULL;
    silbling->right->right = NULL;
    silbling->right->vec = make_rdvec();
    silbling->right->vec.length = silbling->left->vec.length;
    silbling->right->vec.data[silbling->right->vec.length - 1] = -1;
}
