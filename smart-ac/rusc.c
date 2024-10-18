#include "rusc.h"
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
pairing_t pairing; // Pairing that should be in PK is placed in global scope so that it can be linked correctly by other compiled modules.

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
void policy_init(EV *ev, IP *ip, PK pk, M *m, char *pp, bool is_update)
{

    // breadth_first_traversal(root, display, NULL);
    if (!is_update)
    {
        TreeNode *root = get_complete_tree(pp);
        get_W_rho(&(ev->W), &(ev->rho), root);
        element_init_GT(*m, pairing);
        element_random(*m);
    }

    int L = ev->W.rows;
    printf("L is %d\n", L);
    rdmat_mp vec_v = make_rdmat_mp(L, 1);
    for (int i = 0; i < ev->W.cols; i++)
    {
        element_random(vec_v.elem[i]);
    }
    if (!is_update)
    {
        ip->lambda = get_lambda(ev->W, ev->rho, vec_v);
        ip->W = ev->W;
    }

    print_list("ev->rho", ev->rho);
    rdmat_print("ev->W", ev->W);

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
    if (is_update)
    {
        element_set(ev->C, tmp3);
    }
    else
    {
        element_mul(ev->C, *m, tmp3);
    }

    element_init_G1(ev->C0, pairing);
    element_pow_zn(ev->C0, pk.g, vec_v.elem[0]);
    ev->CX_ = initHashMap();
    for (int i = 0; i < ev->rho.length; i++)
    {
        char *attribute = ((TreeNode *)ev->rho.elem_[i])->value;
        element_t *tmpC = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC[0], pairing);
        element_pow_zn(tmpC[0], pk.w, *(element_t *)map_search(ip->lambda, attribute));
        element_pow_zn(tmp1, pk.v, t_.elem[i]);
        element_mul(tmpC[0], tmpC[0], tmp1);
        element_init_G1(tmpC[1], pairing);
        element_from_hash(tmp2, attribute, strlen(attribute));
        element_pow_zn(tmpC[1], pk.u, tmp2);
        element_mul(tmpC[1], tmpC[1], pk.h);
        element_neg(tmp2, t_.elem[i]);
        element_pow_zn(tmpC[1], tmpC[1], tmp2);

        element_init_G1(tmpC[2], pairing);
        element_pow_zn(tmpC[2], pk.g, t_.elem[i]);

        map_insert(ev->CX_, attribute, tmpC);
    }

    element_clear(tmp1);
    element_clear(tmp2);
    element_clear(tmp3);
    free_rdmat_mp(vec_v);
    free_rdmat_mp(t_);
    // rd_free_tree(root);
}
void key_dist(SK *sk, PK pk, MK mk, char **s, int my_attr_size)
{
    element_t *r_ = (element_t *)malloc(sizeof(element_t) * (my_attr_size + 1));
    int offset = 0;

    sk->KX_ = initHashMap();

    element_init_Zr(r_[my_attr_size], pairing);
    element_random(r_[my_attr_size]);
    element_t neg_r, v2neg_r, attr;
    element_init_Zr(neg_r, pairing);
    element_neg(neg_r, r_[my_attr_size]);
    element_init_G1(v2neg_r, pairing);
    element_pow_zn(v2neg_r, pk.v, neg_r);
    element_init_Zr(attr, pairing);

    for (int i = 0; i < my_attr_size; i++)
    {
        char *attribute = s[i];
        element_t *tmp = (element_t *)malloc(2 * sizeof(element_t));
        element_init_Zr(r_[i], pairing);
        element_random(r_[i]);

        element_init_G1(tmp[0], pairing);
        element_pow_zn(tmp[0], pk.g, r_[i]);

        element_init_G1(tmp[1], pairing);

        element_from_hash(attr, attribute, strlen(attribute));

        element_pow_zn(tmp[1], pk.u, attr);

        element_mul(tmp[1], tmp[1], pk.h);

        element_pow_zn(tmp[1], tmp[1], r_[i]);

        element_mul(tmp[1], tmp[1], v2neg_r);

        map_insert(sk->KX_, attribute, tmp);
    }

    element_init_G1(sk->K0, pairing);
    element_init_G1(sk->K1, pairing);
    element_t tmp_exp;
    element_init_G1(tmp_exp, pairing);
    element_pow_zn(sk->K0, pk.g, mk);
    element_pow_zn(tmp_exp, pk.w, r_[my_attr_size]);
    element_mul(sk->K0, sk->K0, tmp_exp);
    element_pow_zn(sk->K1, pk.g, r_[my_attr_size]);

    element_clear(neg_r);
    element_clear(v2neg_r);
    element_clear(attr);
    element_clear(tmp_exp);
    for (int i = 0; i < my_attr_size + 1; i++)
    {
        element_clear(r_[i]);
    }
    free(r_);
}
int verify(M m, EV ev, SK sk, char **s, int my_attr_size)
{
    // element_t m;
    // element_init_GT(m, pairing);
    // element_from_hash(m, _m, strlen(_m));

    int *line_it_has = (int *)malloc(sizeof(ev.rho) * sizeof(int));
    int offset = 0;
    for (int i = 0; i < ev.rho.length; i++)
    {
        // printf("Checking %s\n", ((TreeNode *)ev.rho.elem_[i])->value);
        if (map_search(sk.KX_, ((TreeNode *)ev.rho.elem_[i])->value) != NULL)
        {
            line_it_has[offset++] = i;
        }
    }
    rdmat rows_picked = pick_rows(offset, ev.W, line_it_has);
    rdmat_print("rows_picked", rows_picked);

    rdmat W_T = transpose(rows_picked);
    rdmat_print("W_T", W_T);
    rdmat arged_mat = get_arged_mat(W_T);
    rdmat_print("arged_mat", arged_mat);
    rdmat_f omega = gaussian_elimination(arged_mat);
    rdmat_f_print("omega", omega);

    element_t B_i, prod, B, omega_mp;
    element_init_GT(B_i, pairing);
    element_init_GT(prod, pairing);
    element_init_GT(B, pairing);
    element_init_Zr(omega_mp, pairing);
    element_set1(B);

    for (int i = 0; i < offset; i++)
    {
        const char *attribute = ((TreeNode *)ev.rho.elem_[line_it_has[i]])->value;
        puts(attribute);
        element_t *tmpK = (element_t *)map_search(sk.KX_, attribute);
        element_t *tmpC = (element_t *)map_search(ev.CX_, attribute);

        pairing_apply(B_i, tmpC[0], sk.K1, pairing);
        pairing_apply(prod, tmpC[1], tmpK[0], pairing);
        element_mul(B_i, B_i, prod);
        pairing_apply(prod, tmpC[2], tmpK[1], pairing);
        element_mul(B_i, B_i, prod);

        element_set_si(omega_mp, (int)omega.elem[line_it_has[i]]);
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
    free_rdmat(rows_picked);
    free_rdmat(W_T);
    free_rdmat(arged_mat);
    free_rdmat_f(omega);
    return result;
}
/*
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
*/
void del(EV *ev, int index);
void add(EV *ev, int index, int trace_back, const char *connector, const char *value);
void policy_mod(UEV *uev, PK pk, IP *ip, EV *ev, EV *ev_new, char *pp_new)
{
    TreeNode *new_root = get_complete_tree(pp_new);
    // breadth_first_traversal(new_root, display, NULL);
    rdmat new_W;
    ptr_list new_rho;
    get_W_rho(&new_W, &new_rho, new_root);

    rdmat_print("new_W", new_W);
    print_list("new_rho", new_rho);

    Result result = get_result(ev->rho, new_rho);
    uev->states = initHashMap();
    uev->CX_ = initHashMap();
    for (int i = 0; i < result.the_universe.length; i++)
    {
        const char *attribute = ((TreeNode *)result.the_universe.elem_[i])->value;
        element_t *tmpC_uev = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC_uev[0], pairing);
        element_init_G1(tmpC_uev[1], pairing);
        element_init_G1(tmpC_uev[2], pairing);
        element_t *tmpC_ev = (element_t *)map_search(ev->CX_, attribute);
        if (tmpC_ev == NULL)
        {
            element_set1(tmpC_uev[0]);
            element_set1(tmpC_uev[1]);
            element_set1(tmpC_uev[2]);
        }
        else
        {
            element_set(tmpC_uev[0], tmpC_ev[0]);
            element_set(tmpC_uev[1], tmpC_ev[1]);
            element_set(tmpC_uev[2], tmpC_ev[2]);
        }
        map_insert(uev->CX_, attribute, tmpC_uev);
    }
    for (int i = 0; i < result.deleted_attributes_connected_by_or.length; i++)
    {
        TreeNode *this_node = (TreeNode *)result.deleted_attributes_connected_by_or.elem_[i];
        element_t *tmpC = (element_t *)map_search(uev->CX_, this_node->value);
        element_set1(tmpC[0]);
        element_set1(tmpC[1]);
        element_set1(tmpC[2]);
        state_update(uev->states, this_node, LABEL_DELETE);
        element_t *_lambda_A = (element_t *)map_search(ip->lambda, this_node->value);
        element_set0(*_lambda_A);
    }
    for (int i = 0; i < result.deleted_attributes_connected_by_and.length; i++)
    {
        TreeNode *this_node = (TreeNode *)result.deleted_attributes_connected_by_and.elem_[i];
        element_t *tmpC = (element_t *)map_search(uev->CX_, this_node->value);
        element_set1(tmpC[0]);
        element_set1(tmpC[1]);
        element_set1(tmpC[2]);
        element_t *_lambda_A = (element_t *)map_search(ip->lambda, this_node->value);
        ptr_list the_affected = get_the_affected(this_node);

        element_t tmp1, tmp2, tmp3;
        element_init_G1(tmp1, pairing);
        element_init_G1(tmp2, pairing);
        element_init_Zr(tmp3, pairing);
        element_t t_j;
        element_init_Zr(t_j, pairing);
        for (int j = 0; j < the_affected.length; j++)
        {
            TreeNode *inner_node = (TreeNode *)the_affected.elem_[j];
            element_t *_lambda_j = (element_t *)map_search(ip->lambda, inner_node->value);
            element_add(*_lambda_j, *_lambda_j, *_lambda_A);

            element_t *tmpCp = map_search(uev->CX_, inner_node->value);
            element_random(t_j);
            element_pow_zn(tmp1, pk.w, *_lambda_A);
            element_pow_zn(tmp2, pk.v, t_j);
            element_mul(tmp1, tmp1, tmp2);
            element_mul(tmpCp[0], tmpCp[0], tmp1);

            element_from_hash(tmp3, inner_node->value, strlen(inner_node->value));
            element_pow_zn(tmp1, pk.u, tmp3);
            element_mul(tmp1, tmp1, pk.h);
            element_neg(tmp3, t_j);
            element_pow_zn(tmp1, tmp1, tmp3);
            element_mul(tmpCp[1], tmpCp[1], tmp1);

            element_pow_zn(tmp1, pk.g, t_j);
            element_mul(tmpCp[2], tmpCp[2], tmp1);

            state_update(uev->states, inner_node, LABEL_MULTIPLY);
        }
        element_clear(t_j);
        element_clear(tmp1);
        element_clear(tmp2);
        element_clear(tmp3);

        state_update(uev->states, this_node, LABEL_DELETE);
        element_set0(*_lambda_A);
    }
    puts("Flag in mode4");
    for (int i = 0; i < result.added_attributes_connected_by_or.length; i++)
    {
        TreeNode *this_node = (TreeNode *)result.added_attributes_connected_by_or.elem_[i];
        TreeNode *sibling = get_sibling(this_node);
        element_t *_lambda_A = (element_t *)malloc(sizeof(element_t));
        element_init_Zr(*_lambda_A, pairing);
        element_set(*_lambda_A, *(element_t *)map_search(ip->lambda, sibling->value));
        map_insert(ip->lambda, this_node->value, (void *)_lambda_A);

        element_t *tmpC = (element_t *)map_search(uev->CX_, this_node->value);

        element_t t_A;
        element_init_Zr(t_A, pairing);
        element_random(t_A);
        element_t tmp1, tmp2, tmp3;
        element_init_G1(tmp1, pairing);
        element_init_G1(tmp2, pairing);
        element_init_Zr(tmp3, pairing);
        element_pow_zn(tmp1, pk.w, *_lambda_A);
        element_pow_zn(tmp2, pk.v, t_A);
        element_mul(tmpC[0], tmp1, tmp2);

        element_from_hash(tmp3, this_node->value, strlen(this_node->value));
        element_pow_zn(tmp1, pk.u, tmp3);
        element_mul(tmp1, tmp1, pk.h);
        element_neg(tmp3, t_A);
        element_pow_zn(tmpC[1], tmp1, tmp3);

        element_pow_zn(tmpC[2], pk.g, t_A);

        state_update(uev->states, this_node, LABEL_ADD);

        element_clear(t_A);
        element_clear(tmp1);
        element_clear(tmp2);
        element_clear(tmp3);
    }
    for (int i = result.added_attributes_connected_by_and.length - 1; i >= 0; i--)
    {
        TreeNode *this_node = (TreeNode *)result.added_attributes_connected_by_and.elem_[i];
        element_t *_lambda_A = (element_t *)malloc(sizeof(element_t));
        element_t y_A;
        element_init_Zr(*_lambda_A, pairing);
        element_init_Zr(y_A, pairing);
        element_random(y_A);
        element_neg(*_lambda_A, y_A);
        map_insert(ip->lambda, this_node->value, _lambda_A);

        element_t *tmpC = (element_t *)map_search(uev->CX_, this_node->value);

        element_t t_A;
        element_init_Zr(t_A, pairing);
        element_random(t_A);
        element_t tmp1, tmp2, tmp3;
        element_init_G1(tmp1, pairing);
        element_init_G1(tmp2, pairing);
        element_init_Zr(tmp3, pairing);

        element_pow_zn(tmp1, pk.w, *_lambda_A);
        element_pow_zn(tmp2, pk.v, t_A);
        element_mul(tmpC[0], tmp1, tmp2);

        element_from_hash(tmp3, this_node->value, strlen(this_node->value));
        element_pow_zn(tmp1, pk.u, tmp3);
        element_mul(tmp1, tmp1, pk.h);
        element_neg(tmp3, t_A);
        element_pow_zn(tmpC[1], tmp1, tmp3);

        element_pow_zn(tmpC[2], pk.g, t_A);
        TreeNode *sibling = get_sibling(this_node);
        // if (!is_connector(sibling->value) && map_search(ip->lambda, sibling->value) == NULL)
        //     continue;

        ptr_list the_affected = get_the_affected(this_node);
        element_t t_j;
        element_init_Zr(t_j, pairing);
        for (int j = 0; j < the_affected.length; j++)
        {
            TreeNode *inner_node = (TreeNode *)the_affected.elem_[j];
            element_t *_lambda_j = (element_t *)map_search(ip->lambda, inner_node->value);
            if (_lambda_j == NULL)
            {
                _lambda_j = (element_t *)malloc(sizeof(element_t));
                element_init_Zr(*_lambda_j, pairing);
                element_set0(*_lambda_j);
                map_insert(ip->lambda, inner_node->value, _lambda_j);
            }
            element_add(*_lambda_j, *_lambda_j, y_A);

            element_t *tmpCp = map_search(uev->CX_, inner_node->value);

            element_random(t_j);
            element_pow_zn(tmp1, pk.w, y_A);
            element_pow_zn(tmp2, pk.v, t_j);
            element_mul(tmp1, tmp1, tmp2);
            element_mul(tmpCp[0], tmpCp[0], tmp1);

            element_from_hash(tmp3, inner_node->value, strlen(inner_node->value));
            element_pow_zn(tmp1, pk.u, tmp3);
            element_mul(tmp1, tmp1, pk.h);
            element_neg(tmp3, t_j);
            element_pow_zn(tmp1, tmp1, tmp3);
            element_mul(tmpCp[1], tmpCp[1], tmp1);

            element_pow_zn(tmp1, pk.g, t_j);
            element_mul(tmpCp[2], tmpCp[2], tmp1);

            state_update(uev->states, inner_node, LABEL_MULTIPLY);
        }

        state_update(uev->states, this_node, LABEL_ADD);
        element_clear(y_A);
        element_clear(t_A);
        element_clear(t_j);
        element_clear(tmp1);
        element_clear(tmp2);
        element_clear(tmp3);
    }
    for (int i = 0; i < result.the_remains.length; i++)
    {
        TreeNode *this_node = (TreeNode *)result.the_remains.elem_[i];
        state_update(uev->states, this_node, LABEL_NOP);
    }

    for (int i = 0; i < new_rho.length; i++)
    {
        const char *attribute = ((TreeNode *)new_rho.elem_[i])->value;
        State this_state = (State)map_search(uev->states, attribute);
        if (this_state == STATE_ADD_ || this_state == STATE_MULTIPLY_)
        {
            map_update(uev->states, attribute, (void *)STATE_REPLACE);
        }
        // print_state(attribute, (State)map_search(uev->states, attribute));
    }

    // puts("Updating");
    // for (int i = 0; i < new_rho.length; i++)
    // {
    //     char *attribute = ((TreeNode *)new_rho.elem_[i])->value;
    //     printf("Into %dth loop, name is %s, &lambda is %p\n", i, attribute, map_search(ip->lambda, attribute));
    //     element_printf("lambda is %B\n", *(element_t *)map_search(ip->lambda, attribute));
    // }
    ev_new->rho = new_rho;
    ev_new->W = new_W;
    ip->W = new_W;
    policy_init(ev_new, ip, pk, NULL, pp_new, true);
    for (int i = 0; i < new_rho.length; i++)
    {
        const char *attribute = ((TreeNode *)new_rho.elem_[i])->value;
        element_t *tmpC_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *tmpC_ev_new = (element_t *)map_search(ev_new->CX_, attribute);
        element_mul(tmpC_uev[0], tmpC_uev[0], tmpC_ev_new[0]);
        element_mul(tmpC_uev[1], tmpC_uev[1], tmpC_ev_new[1]);
        element_mul(tmpC_uev[2], tmpC_uev[2], tmpC_ev_new[2]);
    }

    // for (int i = 0; i < new_rho.length; i++)
    // {
    //     const char *attribute = ((TreeNode *)new_rho.elem_[i])->value;
    //     element_t *tmpC = (element_t *)map_search(uev->CX_, attribute);
    //     element_t *lambda = (element_t *)map_search(ip->lambda, attribute);
    //     printf("CX[%s]:\n", attribute);
    //     element_printf("C0:%B\n", tmpC[0]);
    //     element_printf("C1:%B\n", tmpC[1]);
    //     element_printf("C2:%B\n", tmpC[2]);
    //     element_printf("lamnda[%s]: %B\n", attribute, *lambda);
    // }
    // exit(1);
}

void evidence_mod(EV *ev_new, EV *ev_cur, UEV *uev)
{
    for (int i = 0; i < ev_new->rho.length; i++)
    {
        const char *attribute = ((TreeNode *)ev_new->rho.elem_[i])->value;
        element_mul(ev_new->C, ev_new->C, ev_cur->C);
        element_mul(ev_new->C0, ev_new->C0, ev_cur->C0);
        puts(attribute);

        element_t *_CX_in_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *_CX_in_ev_new = (element_t *)map_search(ev_new->CX_, attribute);
        switch ((State)map_search(uev->states, attribute))
        {
        case STATE_MULTIPLY:
            element_mul(_CX_in_ev_new[0], _CX_in_ev_new[0], _CX_in_uev[0]);
            element_mul(_CX_in_ev_new[1], _CX_in_ev_new[1], _CX_in_uev[1]);
            element_mul(_CX_in_ev_new[2], _CX_in_ev_new[2], _CX_in_uev[2]);
            break;
        case STATE_REPLACE:
            element_set(_CX_in_ev_new[0], _CX_in_uev[0]);
            element_set(_CX_in_ev_new[1], _CX_in_uev[1]);
            element_set(_CX_in_ev_new[2], _CX_in_uev[2]);
            break;
        case STATE_DELETE:
            map_remove(ev_new->CX_, attribute);
            break;
        case STATE_ADD:
            map_insert(ev_new->CX_, attribute, _CX_in_uev);
            break;
        default:
            break;
        }
    }
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
    if (is_or(node->parent->value))
    {
        // element_clear(ev->C1_[index]);
        // element_clear(ev->C2_[index]);
        // element_clear(ev->C3_[index]);
    }
    else if (is_and(node->parent->value))
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
    if (is_connector(connector))
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
        if (is_or(connector))
        {
        }
        else if (is_and(connector))
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

bool state_update(HashMap *_map, TreeNode *_node, Label _label)
{
    State this_state = (State)map_search(_map, _node->value);
    if (this_state == 0)
    {
        map_insert(_map, _node->value, (void *)STATE_START);
    }
    this_state = (State)map_search(_map, _node->value);
    map_update(_map, _node->value, (void *)transition(this_state, _label));
}

HashMap *get_lambda(rdmat a, ptr_list rho, rdmat_mp b)
{
    HashMap *map = initHashMap();
    rdmat_mp c = make_rdmat_mp(a.rows, b.cols);
    element_t prod;
    element_init_Zr(prod, pairing);
    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < b.cols; j++)
        {
            element_set0(c.elem[i * b.cols + j]);
            for (int k = 0; k < a.cols; k++)
            {
                element_mul_si(prod, b.elem[k * b.cols + j], a.elem[i][k]);
                element_add(c.elem[i * c.cols + j], c.elem[i * c.cols + j], prod);
            }
        }
    }
    element_clear(prod);
    for (int i = 0; i < rho.length; i++)
    {
        map_insert(map, ((TreeNode *)rho.elem_[i])->value, c.elem[i]);
    }
    return map;
}

Result get_result(ptr_list rho1, ptr_list rho2)
{
    Result result = {
        .deleted_attributes_connected_by_or = make_ptr_list(0),
        .deleted_attributes_connected_by_and = make_ptr_list(0),
        .added_attributes_connected_by_or = make_ptr_list(0),
        .added_attributes_connected_by_and = make_ptr_list(0),
        .the_remains = make_ptr_list(0),
        .the_universe = make_ptr_list(0),
    };

    for (int i = 0; i < rho1.length; i++)
    {
        TreeNode *this_node = (TreeNode *)rho1.elem_[i];
        result.the_universe = add_to_list(result.the_universe, this_node);
        if (find_in(this_node, rho2) == NULL)
        {
            if (is_or(this_node->parent->value))
            {
                result.deleted_attributes_connected_by_or = add_to_list(result.deleted_attributes_connected_by_or, this_node);
            }
            else
            {
                result.deleted_attributes_connected_by_and = add_to_list(result.deleted_attributes_connected_by_and, this_node);
            }
        }
    }
    TreeNode *candidate = NULL;
    for (int i = 0; i < rho2.length; i++)
    {
        TreeNode *this_node = rho2.elem_[i];
        if ((candidate = find_in(this_node, rho1)) == NULL)
        {
            result.the_universe = add_to_list(result.the_universe, this_node);
            if (is_or(this_node->parent->value))
            {
                result.added_attributes_connected_by_or = add_to_list(result.added_attributes_connected_by_or, this_node);
            }
            else
            {
                result.added_attributes_connected_by_and = add_to_list(result.added_attributes_connected_by_and, this_node);
            }
        }
        else
        {
            if (!is_same_path(this_node, candidate))
            {
                if (is_or(candidate->parent->value))
                {
                    result.deleted_attributes_connected_by_or = add_to_list(result.deleted_attributes_connected_by_or, candidate);
                }
                else
                {
                    result.deleted_attributes_connected_by_and = add_to_list(result.deleted_attributes_connected_by_and, candidate);
                }
                if (is_or(this_node->parent->value))
                {
                    result.added_attributes_connected_by_or = add_to_list(result.added_attributes_connected_by_or, this_node);
                }
                else
                {
                    result.added_attributes_connected_by_and = add_to_list(result.added_attributes_connected_by_and, this_node);
                }
            }
            else
            {
                result.the_remains = add_to_list(result.the_remains, candidate);
            }
        }
    }
    /*
    print_list("deleted_attributes_connected_by_or", result.deleted_attributes_connected_by_or);
    print_list("deleted_attributes_connected_by_and", result.deleted_attributes_connected_by_and);
    print_list("added_attributes_connected_by_or", result.added_attributes_connected_by_or);
    print_list("added_attributes_connected_by_and", result.added_attributes_connected_by_and);
    print_list("the_remains", result.the_remains);
    print_list("the_universe", result.the_universe);
    */
    return result;
}

void _find_that_kid(TreeNode *node, void **food)
{
    if (!is_connector(node->value))
    {
        *(ptr_list *)food = add_to_list(*(ptr_list *)food, node);
    }
}
ptr_list get_the_affected(TreeNode *_node)
{
    TreeNode *sibling = get_sibling(_node);
    ptr_list list = make_ptr_list(0);
    if (is_connector(sibling->value))
    {
        breadth_first_traversal(sibling, _find_that_kid, (void **)&list);
    }
    else
    {
        list = add_to_list(list, sibling->value);
    }
    return list;
}
