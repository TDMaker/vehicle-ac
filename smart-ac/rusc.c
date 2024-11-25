#include <include/rusc.h>
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
pairing_t pairing; // Pairing that should be in PK is placed in global scope so that it can be linked correctly by other compiled modules.
TreeNode *root;

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
        root = get_complete_tree(pp);
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
        char *attribute = (char *)ev->rho.elem_[i];
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
    // int offset = 0;

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
    int *line_it_has = (int *)malloc(sizeof(ev.rho) * sizeof(int));
    int offset = 0;
    for (int i = 0; i < ev.rho.length; i++)
    {
        if (map_search(sk.KX_, (char *)ev.rho.elem_[i]) != NULL)
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
    if (omega.rows == 0)
    {
        return 0;
    }
    rdmat_f_print("omega", omega);

    element_t B_i, prod, B, omega_mp;
    element_init_GT(B_i, pairing);
    element_init_GT(prod, pairing);
    element_init_GT(B, pairing);
    element_init_Zr(omega_mp, pairing);
    element_set1(B);

    for (int i = 0; i < offset; i++)
    {
        const char *attribute = (char *)ev.rho.elem_[line_it_has[i]];
        element_t *tmpK = (element_t *)map_search(sk.KX_, attribute);
        element_t *tmpC = (element_t *)map_search(ev.CX_, attribute);

        pairing_apply(B_i, tmpC[0], sk.K1, pairing);
        pairing_apply(prod, tmpC[1], tmpK[0], pairing);
        element_mul(B_i, B_i, prod);
        pairing_apply(prod, tmpC[2], tmpK[1], pairing);
        element_mul(B_i, B_i, prod);

        element_set_si(omega_mp, (int)omega.elem[i]);
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
void policy_mod(UEV *uev, PK pk, IP *ip, EV *ev, char *pp_new)
{
    TreeNode *new_root = get_complete_tree(pp_new);
    // breadth_first_traversal(new_root, display, NULL);
    rdmat new_W;
    ptr_list new_rho;
    get_W_rho(&new_W, &new_rho, new_root);

    rdmat_print("new_W", new_W);
    print_list("new_rho", new_rho);
    RDResult my_result = get_the_deleted(ev->rho, new_rho, root, new_root);
    print_list("the deleted", my_result.the_deleted);
    print_list("the remains", my_result.the_remains);
    uev->states = initHashMap();
    uev->CX_ = initHashMap();

    for (int i = 0; i < ev->rho.length; i++)
    {
        char *attribute = (char *)ev->rho.elem_[i];
        element_t *tmpC_ev = (element_t *)map_search(ev->CX_, attribute);
        element_t *tmpC_uev = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC_uev[0], pairing);
        element_init_G1(tmpC_uev[1], pairing);
        element_init_G1(tmpC_uev[2], pairing);
        element_set(tmpC_uev[0], tmpC_ev[0]);
        element_set(tmpC_uev[1], tmpC_ev[1]);
        element_set(tmpC_uev[2], tmpC_ev[2]);
        map_insert(uev->CX_, attribute, tmpC_uev);
    }

    element_t tmp1, tmp2, tmp3;
    element_init_G1(tmp1, pairing);
    element_init_G1(tmp2, pairing);
    element_init_Zr(tmp3, pairing);
    element_t t_A, t_j, y_A;
    element_init_Zr(t_A, pairing);
    element_init_Zr(t_j, pairing);
    element_init_Zr(y_A, pairing);

    for (int i = 0; i < my_result.the_deleted.length; i++)
    {
        char *attribute = (char *)my_result.the_deleted.elem_[i];
        element_t *tmpC = (element_t *)map_search(uev->CX_, attribute);
        element_clear(tmpC[0]);
        element_clear(tmpC[1]);
        element_clear(tmpC[2]);
        free(tmpC);
        map_remove(uev->CX_, attribute);
        state_update(uev->states, attribute, LABEL_DELETE);

        TreeNode *node2del = find_node_from_tree(attribute, root);
        TreeNode *lvlup_node = node2del->parent;

        int connector = del_from_tree(node2del);
        element_t *_lambda_A = (element_t *)map_search(ip->lambda, attribute);

        if (connector == 1)
        {
            ptr_list the_affected = get_all_under_nodes(lvlup_node);

            for (int j = 0; j < the_affected.length; j++)
            {
                TreeNode *inner_node = (TreeNode *)the_affected.elem_[j];
                element_t *_lambda_j = (element_t *)map_search(ip->lambda, inner_node->value);
                element_add(*_lambda_j, *_lambda_j, *_lambda_A);
                element_t *tmpC_j = map_search(uev->CX_, inner_node->value);
                element_random(t_j);
                element_pow_zn(tmp1, pk.w, *_lambda_A);
                element_pow_zn(tmp2, pk.v, t_j);
                element_mul(tmp1, tmp1, tmp2);
                element_mul(tmpC_j[0], tmpC_j[0], tmp1);

                element_from_hash(tmp3, inner_node->value, strlen(inner_node->value));
                element_pow_zn(tmp1, pk.u, tmp3);
                element_mul(tmp1, tmp1, pk.h);
                element_neg(tmp3, t_j);
                element_pow_zn(tmp1, tmp1, tmp3);
                element_mul(tmpC_j[1], tmpC_j[1], tmp1);

                element_pow_zn(tmp1, pk.g, t_j);
                element_mul(tmpC_j[2], tmpC_j[2], tmp1);

                state_update(uev->states, inner_node->value, LABEL_MULTIPLY);
            }
        }
        element_clear(*_lambda_A);
        map_remove(ip->lambda, attribute);
    }

    // ev->rho = shrink_list(ev->rho);

    Stack *stack = rd_stk_create_stack(100);
    TreeNode *node2del;
    for (int i = 0; i < new_rho.length; i++)
    {
        node2del = find_node_from_tree((char *)new_rho.elem_[i], new_root);
        if (node2del != NULL && !find_attribute_in(node2del->value, my_result.the_remains))
        {
            int path = get_path(node2del);
            if (path != -1)
            {
                int connector = del_from_tree(node2del);
                rd_stk_push(stack, (StkItem){.ptr = (char *)new_rho.elem_[i], .path = path, .connector = connector});
            }
        }
    }

    while (!rd_stk_is_empty(stack))
    {
        StkItem item = rd_stk_pop(stack);
        TreeNode *the_new_added = add_to_tree(new_root, item.path, item.connector, (char *)item.ptr);
        printf("The new added is %s, its sibling is %s, its connector is %s\n", the_new_added->value, get_sibling(the_new_added)->value, item.connector == 0 ? "||" : "&&");

        element_t *tmpC = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC[0], pairing);
        element_init_G1(tmpC[1], pairing);
        element_init_G1(tmpC[2], pairing);

        element_random(t_A);

        if (item.connector == 0) // add or
        {
            TreeNode *sibling = get_sibling(the_new_added);
            element_t *_lambda_A = get_this_lambda(ip->lambda, sibling);
            printf("Get the sibling's lambda is %p\n", _lambda_A);
            element_printf("Inserting %s's lambda =%B\n", the_new_added->value, *_lambda_A);
            map_insert(ip->lambda, the_new_added->value, (void *)_lambda_A);

            element_pow_zn(tmp1, pk.w, *_lambda_A);
            element_pow_zn(tmp2, pk.v, t_A);
            element_mul(tmpC[0], tmp1, tmp2);

            element_from_hash(tmp3, the_new_added->value, strlen(the_new_added->value));
            element_pow_zn(tmp1, pk.u, tmp3);
            element_mul(tmp1, tmp1, pk.h);
            element_neg(tmp3, t_A);
            element_pow_zn(tmpC[1], tmp1, tmp3);

            element_pow_zn(tmpC[2], pk.g, t_A);

            map_insert(uev->CX_, the_new_added->value, (void *)tmpC);
            state_update(uev->states, the_new_added->value, LABEL_ADD);
        }
        else if (item.connector == 1) // add and
        {
            element_t *_lambda_A = (element_t *)malloc(sizeof(element_t));
            element_init_Zr(*_lambda_A, pairing);
            element_random(y_A);
            element_neg(*_lambda_A, y_A);
            map_insert(ip->lambda, the_new_added->value, (void *)_lambda_A);
            // printf("Generating %s's lambda is %p\n", the_new_added->value, _lambda_A);
            // element_printf("Inserting %s's lambda =%B\n", the_new_added->value, *_lambda_A);

            element_pow_zn(tmp1, pk.w, *_lambda_A);
            element_pow_zn(tmp2, pk.v, t_A);
            element_mul(tmpC[0], tmp1, tmp2);

            element_from_hash(tmp3, the_new_added->value, strlen(the_new_added->value));
            element_pow_zn(tmp1, pk.u, tmp3);
            element_mul(tmp1, tmp1, pk.h);
            element_neg(tmp3, t_A);
            element_pow_zn(tmpC[1], tmp1, tmp3);
            element_pow_zn(tmpC[2], pk.g, t_A);
            map_insert(uev->CX_, the_new_added->value, (void *)tmpC);
            state_update(uev->states, the_new_added->value, LABEL_ADD);

            TreeNode *sibling = get_sibling(the_new_added);
            ptr_list the_affected = get_all_under_nodes(sibling);
            print_list("the affected", the_affected);
            for (int j = 0; j < the_affected.length; j++)
            {
                TreeNode *inner_node = (TreeNode *)the_affected.elem_[j];
                // printf("affecting %s\n", inner_node->value);
                element_t *_lambda_j = (element_t *)map_search(ip->lambda, inner_node->value);
                // printf("lambda %s is @%p\n", inner_node->value, _lambda_j);
                // element_printf("=%B\n", _lambda_j);
                element_add(*_lambda_j, *_lambda_j, y_A);
                element_t *tmpC_j = map_search(uev->CX_, inner_node->value);
                // element_printf("%s's lambda turns to\n%B\n", inner_node->value, *_lambda_j);

                element_random(t_j);
                element_pow_zn(tmp1, pk.w, y_A);
                element_pow_zn(tmp2, pk.v, t_j);
                element_mul(tmp1, tmp1, tmp2);
                element_mul(tmpC_j[0], tmpC_j[0], tmp1);
                element_from_hash(tmp3, inner_node->value, strlen(inner_node->value));
                element_pow_zn(tmp1, pk.u, tmp3);
                element_mul(tmp1, tmp1, pk.h);
                element_neg(tmp3, t_j);
                element_pow_zn(tmp1, tmp1, tmp3);
                element_mul(tmpC_j[1], tmpC_j[1], tmp1);

                element_pow_zn(tmp1, pk.g, t_j);
                element_mul(tmpC_j[2], tmpC_j[2], tmp1);

                state_update(uev->states, inner_node->value, LABEL_MULTIPLY);
            }
        }
    }
    element_clear(y_A);
    element_clear(t_A);
    element_clear(t_j);
    element_clear(tmp1);
    element_clear(tmp2);
    element_clear(tmp3);

    for (int i = 0; i < new_rho.length; i++)
    {
        const char *attribute = (char *)new_rho.elem_[i];
        State this_state = (State)map_search(uev->states, attribute);
        if (this_state == STATE_ADD_ || this_state == STATE_MULTIPLY_)
        {
            map_update(uev->states, attribute, (void *)STATE_REPLACE);
        }
        print_state(attribute, (State)map_search(uev->states, attribute));
    }

    EV a_new_ev;
    a_new_ev.rho = new_rho;
    a_new_ev.W = new_W;
    ip->W = new_W;
    uev->rho = new_rho;
    uev->W = new_W;

    // rdmat_print("new_W", new_W);
    // print_list("new_rho", new_rho);

    policy_init(&a_new_ev, ip, pk, NULL, pp_new, true);

    element_init_GT(uev->C, pairing);
    element_init_G1(uev->C0, pairing);
    element_set(uev->C, a_new_ev.C);
    element_set(uev->C0, a_new_ev.C0);
    for (int i = 0; i < new_rho.length; i++)
    {
        const char *attribute = (char *)new_rho.elem_[i];
        element_t *tmpC_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *tmpC_ev_new = (element_t *)map_search(a_new_ev.CX_, attribute);

        element_mul(tmpC_uev[0], tmpC_uev[0], tmpC_ev_new[0]);
        element_mul(tmpC_uev[1], tmpC_uev[1], tmpC_ev_new[1]);
        element_mul(tmpC_uev[2], tmpC_uev[2], tmpC_ev_new[2]);
    }
}

void evidence_mod(EV *ev_cur, UEV *uev)
{
    for (int i = 0; i < uev->rho.length; i++)
    {
        // element_mul(ev_cur->C, ev_cur->C, uev->C);
        // element_mul(ev_cur->C0, ev_cur->C0, uev->C0);
        // element_set(ev_cur->C, uev->C);
        // element_set(ev_cur->C0, uev->C0);

        const char *attribute = (char *)uev->rho.elem_[i];
        element_t *CX_in_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *CX_in_ev_cur = (element_t *)map_search(ev_cur->CX_, attribute);
        // printf("%s's CX_in_uev is %p, CX_in_ev_cur is %p\n", attribute, CX_in_uev, CX_in_ev_cur);
        switch ((State)map_search(uev->states, attribute))
        {
        case STATE_MULTIPLY:
        puts("``````````````````````````````````MUL");
            element_mul(CX_in_ev_cur[0], CX_in_ev_cur[0], CX_in_uev[0]);
            element_mul(CX_in_ev_cur[1], CX_in_ev_cur[1], CX_in_uev[1]);
            element_mul(CX_in_ev_cur[2], CX_in_ev_cur[2], CX_in_uev[2]);
            break;
        case STATE_REPLACE:
        puts("```````````````````````````REPLACE");
            element_set(CX_in_ev_cur[0], CX_in_uev[0]);
            element_set(CX_in_ev_cur[1], CX_in_uev[1]);
            element_set(CX_in_ev_cur[2], CX_in_uev[2]);
            break;
        case STATE_DELETE:
        puts("`````````````````````````````````DEL");
            map_remove(ev_cur->CX_, attribute);
            break;
        case STATE_ADD:
        puts("`````````````````````````````````ADD");
        puts(attribute);
            map_insert(ev_cur->CX_, attribute, (void *)CX_in_uev);
            break;
        default:
            break;
        }
    }
    ev_cur->rho = uev->rho;
    ev_cur->W = uev->W;
}

bool state_update(HashMap *_map, const char *attribute, Label _label)
{
    State this_state = (State)map_search(_map, attribute);
    if (this_state == STATE_NOT_EXISTS)
    {
        map_insert(_map, attribute, (void *)STATE_START);
    }
    this_state = (State)map_search(_map, attribute);
    return map_update(_map, attribute, (void *)transition(this_state, _label));
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
        map_insert(map, (char *)rho.elem_[i], c.elem[i]);
    }
    return map;
}

RDResult get_the_deleted(ptr_list rho1, ptr_list rho2, TreeNode *root1, TreeNode *root2)
{
    RDResult my_result = {
        .the_deleted = make_ptr_list(0),
        .the_remains = make_ptr_list(0),
    };
    for (int i = 0; i < rho1.length; i++)
    {
        char *this_attribute = (char *)rho1.elem_[i];
        if (find_attribute_in(this_attribute, rho2) == NULL)
        {
            my_result.the_deleted = add_to_list(my_result.the_deleted, strdup(this_attribute));
        }
    }
    for (int i = 0; i < rho2.length; i++)
    {
        char *this_attribute = rho2.elem_[i];
        char *candidate = find_attribute_in(this_attribute, rho1);
        if (candidate != NULL)
        {
            if (is_same_path(find_node_from_tree(this_attribute, root2), find_node_from_tree(candidate, root1)))
            {
                my_result.the_remains = add_to_list(my_result.the_remains, strdup(this_attribute));
            }
            else
            {
                my_result.the_deleted = add_to_list(my_result.the_deleted, strdup(this_attribute));
            }
        }
    }
    return my_result;
}

element_t *get_this_lambda(HashMap *lambda, TreeNode *node)
{
    if (is_and(node->value))
    {
        element_t *tmp = (element_t *)malloc(sizeof(element_t));
        element_init_Zr(*tmp, pairing);
        element_t *left = get_this_lambda(lambda, node->left);
        element_t *right = get_this_lambda(lambda, node->right);
        element_add(*tmp, *left, *right);
        element_clear(*left);
        element_clear(*right);
        return tmp;
    }
    else if (is_or(node->value))
    {
        if (is_connector(node->left->value))
            return get_this_lambda(lambda, node->right);
        return get_this_lambda(lambda, node->left);
    }
    else
    {
        element_t *tmp = (element_t *)malloc(sizeof(element_t));
        element_init_Zr(*tmp, pairing);
        element_set(*tmp, *(element_t *)map_search(lambda, node->value));
        return tmp;
    }
}
