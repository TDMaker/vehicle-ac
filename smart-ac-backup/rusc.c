#include "rusc.h"
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
pairing_t pairing; // Pairing that should be in PK is placed in global scope so that it can be linked correctly by other compiled modules.

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
    TreeNode *root = get_complete_tree(pp);
    breadth_first_traversal(root, display, NULL);
    get_W_rho(&(ev->W), &(ev->rho), root);
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

    ip->lambda = rdmat_mul_sp_mp(ev->W, vec_v);
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
    ev->C1_ = (element_t **)malloc(sizeof(element_t *) * L);
    ev->C2_ = (element_t **)malloc(sizeof(element_t *) * L);
    ev->C3_ = (element_t **)malloc(sizeof(element_t *) * L);
    for (int i = 0; i < L; i++)
    {
        ev->C1_[i] = (element_t *)malloc(sizeof(element_t));
        element_init_G1(*ev->C1_[i], pairing);
        element_pow_zn(*ev->C1_[i], pk.w, *ip->lambda[i]);
        element_pow_zn(tmp1, pk.v, t_.elem[i]);
        element_mul(*ev->C1_[i], *(ev->C1_[i]), tmp1);

        ev->C2_[i] = (element_t *)malloc(sizeof(element_t));
        element_init_G1(*ev->C2_[i], pairing);
        element_set_si(tmp2, i);
        element_pow_zn(*ev->C2_[i], pk.u, tmp2);
        element_mul(*ev->C2_[i], *ev->C2_[i], pk.h);
        element_neg(tmp2, t_.elem[i]);
        element_pow_zn(*ev->C2_[i], *ev->C2_[i], tmp2);

        ev->C3_[i] = (element_t *)malloc(sizeof(element_t));
        element_init_G1(*ev->C3_[i], pairing);
        element_pow_zn(*ev->C3_[i], pk.g, t_.elem[i]);
    }

    element_clear(tmp1);
    element_clear(tmp2);
    element_clear(tmp3);
    element_clear(m);
    free_rdmat_mp(vec_v);
    free_rdmat_mp(t_);
    // rd_free_tree(root);
}
void key_dist(SK *sk, PK pk, MK mk, ptr_list rho, char **s, int my_attr_size)
{
    int S[1024] = {-1};
    sk->len_s = 0;
    for (int i = 0; i < rho.length; i++)
    {
        for (int j = 0; j < my_attr_size; j++)
        {
            if (strcmp(((TreeNode *)rho.elem_[i])->value, s[j]) == 0)
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
    rdmat arged_mat = get_arged_mat(W_T);
    rdmat_f omega = gaussian_elimination(arged_mat);
    rdmat_f_print("omega", omega);

    element_t B_i, prod, B, omega_mp;
    element_init_GT(B_i, pairing);
    element_init_GT(prod, pairing);
    element_init_GT(B, pairing);
    element_init_Zr(omega_mp, pairing);
    element_set1(B);

    for (int i = 0; i < sk.len_s; i++)
    {
        pairing_apply(B_i, *ev.C1_[sk.s[i]], sk.K1, pairing);
        pairing_apply(prod, (*ev.C2_[sk.s[i]]), sk.K2_[i], pairing);
        element_mul(B_i, B_i, prod);
        pairing_apply(prod, *ev.C3_[sk.s[i]], sk.K3_[i], pairing);
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
    free_rdmat(arged_mat);
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
    free(ev->rho.elem_);
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
        element_clear(*ev->C1_[i]);
        element_clear(*ev->C2_[i]);
        element_clear(*ev->C3_[i]);
        // element_clear(*ip->lambda[i]);
    }
    free(ev->C1_);
    free(ev->C2_);
    free(ev->C3_);
    free(ip->lambda);

    pairing_clear(pairing);
}
void del(EV *ev, int index);
void add(EV *ev, int index, int trace_back, const char *connector, const char *value);
void policy_mod(UEV *uev, IP *ip_new, PK pk, IP *ip, EV *ev, char *pp_new)
{
    TreeNode *new_root = get_complete_tree(pp_new);
    breadth_first_traversal(new_root, display, NULL);
    rdmat new_W;
    ptr_list new_rho;
    get_W_rho(&new_W, &new_rho, new_root);

    rdmat_print("new_W", new_W);
    State new = transition(STATE_START, LABEL_ADD);
    printf("%d\n", new);

    // ptr_list del_list = get_diff(ev->rho, new_rho);
    Result result = get_result(ev->rho, new_rho);
    for(int i = 0; i < result.deleted_attributes_connected_by_or.length; i++)
    {
        
    }







    return;
    int row_removed = -1;
    for (int i = 0; i < ev->rho.length; i++)
    {
        int has = 0;
        for (int j = 0; j < new_rho.length; j++)
        {
            if (ev->rho.elem_[i] == new_rho.elem_[j])
            {
                printf("ev->rho.node_[%d].val = %s\n", i, ((TreeNode **)ev->rho.elem_)[i]->value);
                has = 1;
                break;
            }
        }
        if (has == 0)
        {
            row_removed = i;
            break;
        }
    }

    printf("The %dth node has been removed.\n", row_removed);

    int **tmp_W = (int **)malloc(sizeof(int *) * ev->W.rows);
    for (int i = 0; i < ev->W.rows; i++)
    {
        tmp_W[i] = NULL;
    }

    for (int i = 0; i < ev->rho.length; i++)
    {
        for (int j = 0; j < new_rho.length; j++)
        {
            if (ev->rho.elem_[i] == new_rho.elem_[j])
            {
                tmp_W[i] = ev->W.elem[j];
                break;
            }
        }
    }
    for (int i = 0; i < new_rho.length; i++)
    {
        if (tmp_W[i] == NULL)
        {
            for (int j = i; j < new_rho.length; j++)
            {
                tmp_W[j] = tmp_W[j + 1];
            }
        }
    }
    free(ev->W.elem);
    ev->W.elem = tmp_W;
    rdmat_print("new W", ev->W);

    element_t **new_C1_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    element_t **new_C2_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    element_t **new_C3_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    element_t **new_lambda = (element_t **)malloc(sizeof(element_t *) * new_rho.length);

    for (int i = 0, j = 0; i < ev->rho.length; i++, j++)
    {
        if (i == row_removed)
        {
            element_free(*ev->C1_[i]);
            element_free(*ev->C2_[i]);
            element_free(*ev->C3_[i]);
            element_free(*ip->lambda[i]);
            j--;
            continue;
        }
        new_C1_[j] = ev->C1_[i];
        new_C2_[j] = ev->C2_[i];
        new_C3_[j] = ev->C3_[i];
        new_lambda[j] = ip->lambda[i];
    }

    ev->C1_ = new_C1_;
    ev->C2_ = new_C2_;
    ev->C3_ = new_C3_;
    // ip->lambda = new_lambda;

    // add_or(root, "F");
    // TODO:
    // get_W_rho();
    // breadth_first_traversal(root, display, NULL);
}

void evidence_mod(EV *ev_new, EV ev, UEV uev)
{
}

void del(EV *ev, int index)
{
    if (index >= ev->rho.length)
    {
        puts("Index exceeds boundary!");
        return;
    }
    TreeNode *node = ev->rho.elem_[index];
    TreeNode *sibling = node->parent->left == node ? node->parent->right : node->parent->left;
    if (strcmp(node->parent->value, "||") == 0)
    {
        // element_clear(ev->C1_[index]);
        // element_clear(ev->C2_[index]);
        // element_clear(ev->C3_[index]);
    }
    else if (strcmp(node->parent->value, "&&") == 0)
    {
        for (int i = 0; i < sibling->vec.length; i++)
        {
            sibling->vec.data[i] += node->vec.data[i];
        }
        // sibling->vec.length--;
    }
    else
    {
        puts("The parent of the handling node is not a connector!");
        return;
    }

    strcpy(sibling->parent->value, sibling->value);
    free_rdvec(sibling->parent->vec);
    sibling->parent->vec = sibling->vec;
    sibling->parent->left = sibling->left;
    sibling->parent->right = sibling->right;
    free(sibling);
    free_rdvec(node->vec);
    free(node);

    // update the address recorded in the old rho.
    for (int i = 0; i < ev->rho.length; i++)
    {
        if (ev->rho.elem_[i] == sibling)
        {
            ev->rho.elem_[i] = sibling->parent;
            break;
        }
    }
}

void add(EV *ev, int index, int trace_back, const char *connector, const char *value)
{
    if (index >= ev->rho.length)
    {
        puts("Index exceeds boundary!");
        return;
    }
    TreeNode *new_parent = ev->rho.elem_[index];
    for (int i = 0; i < trace_back; i++)
    {
        if (new_parent->parent != NULL)
        {
            new_parent = new_parent->parent;
        }
        else
        {
            puts("Can't find the ancestors that far away.");
            return;
        }
    }
    if (strcmp(connector, "||") == 0 || strcmp(connector, "&&") == 0)
    {
        TreeNode *sibling = (TreeNode *)malloc(sizeof(TreeNode));
        memcpy(sibling, new_parent, sizeof(TreeNode));
        if (new_parent->left != NULL)
            new_parent->left->parent = sibling;
        if (new_parent->right != NULL)
            new_parent->right->parent = sibling;
        new_parent->right = sibling;
        new_parent->left = (TreeNode *)malloc(sizeof(TreeNode));
        sibling->parent = new_parent;
        sibling->vec = cpy_rdvec(new_parent->vec);
        strcpy(new_parent->value, connector);
        new_parent->left->parent = new_parent;
        strcpy(new_parent->left->value, value);
        new_parent->left->left = NULL;
        new_parent->left->right = NULL;
        if (strcmp(connector, "||") == 0)
        {
        }
        else if (strcmp(connector, "&&") == 0)
        {
            sibling->vec.data[sibling->vec.length] = 1;
            sibling->vec.length++;
            new_parent->left->vec = make_rdvec();
            new_parent->left->vec.length = new_parent->right->vec.length;
            new_parent->left->vec.data[new_parent->left->vec.length - 1] = -1;
        }
        // print_node(new_parent);
        // print_node(new_parent->left);
        // print_node(new_parent->right);
    }
    else
    {
        puts("Unknown connector to be added!");
        return;
    }
}
