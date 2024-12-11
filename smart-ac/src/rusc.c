#include <rusc.h>
#define BUFFER_SIZE 1024
static char buffer[BUFFER_SIZE];
pairing_t pairing; // Pairing that should be in PK is placed in global scope so that it can be linked correctly by other compiled modules.
TreeNode *root;
const char *UNIVERS[] = {"A", "B", "C", "D", "E", "F", "G", "H", "Y", "Z"};

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
void policy_init(EV *ev, IP *ip, PK pk, M *m, char *pp)
{
    // breadth_first_traversal(root, display, NULL);
    if (m)
    {
        root = get_complete_tree(pp);
        get_W_rho(&(ev->W), &(ev->attrs), root);
        element_init_GT(*m, pairing);
        element_random(*m);
    }

    int L = ev->W.rows;
    printf("L is %d\n", L);
    rdmat_mp vec_v = make_rdmat_mp(L, 1);
    rdmat_mp t_ = make_rdmat_mp(1, L);
    for (int i = 0; i < L; i++)
    {
        element_random(vec_v.elem[i]);
        element_random(t_.elem[i]);
    }
    HashMap *lambda = get_lambda(ev->W, ev->attrs, vec_v);
    rdmat_print("ev->W", ev->W);

    element_t tmp1, tmp2, tmp3;
    element_init_G1(tmp1, pairing);
    element_init_Zr(tmp2, pairing);
    element_init_GT(tmp3, pairing);

    element_init_GT(ev->C, pairing);
    element_pow_zn(tmp3, pk.frag, vec_v.elem[0]);

    m ? element_mul(ev->C, *m, tmp3) : element_set(ev->C, tmp3);

    element_init_G1(ev->C0, pairing);
    element_pow_zn(ev->C0, pk.g, vec_v.elem[0]);
    ev->CX_ = initHashMap();
    for (int i = 0; i < L; i++)
    {
        char *attribute = (char *)ev->attrs.elem_[i];
        element_t *tmpC = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC[0], pairing);
        element_pow_zn(tmp1, pk.v, t_.elem[i]);
        element_pow_zn(tmpC[0], pk.w, *(element_t *)map_search(lambda, attribute));
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

    //-----

    // element_t *TMP_CX_Z = map_search(ev->CX_, "Z");
    // element_t *TMP_CX_G = map_search(ev->CX_, "G");
    // element_t *TMP_CX_E;
    // if (is_update)
    //     TMP_CX_E = map_search(ev->CX_, "Y");
    // else
    //     TMP_CX_E = map_search(ev->CX_, "E");

    // element_printf("C1Z = %B\n", TMP_CX_Z[0]);
    // element_printf("C2Z = %B\n", TMP_CX_Z[1]);
    // element_printf("C3Z = %B\n", TMP_CX_Z[2]);
    // element_printf("C1G = %B\n", TMP_CX_G[0]);
    // element_printf("C2G = %B\n", TMP_CX_G[1]);
    // element_printf("C3G = %B\n", TMP_CX_G[2]);
    // element_printf("C1E = %B\n", TMP_CX_E[0]);
    // element_printf("C2E = %B\n", TMP_CX_E[1]);
    // element_printf("C3E = %B\n", TMP_CX_E[2]);

    //------

    if (m)
    {
        // TODO: clear the lambda new generated if necessary, later.
        ip->lambda = lambda;
        ip->W = ev->W;
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
    int *line_it_has = (int *)malloc(sizeof(ev.attrs) * sizeof(int));
    int offset = 0;
    for (int i = 0; i < ev.attrs.length; i++)
    {
        if (map_search(sk.KX_, (char *)ev.attrs.elem_[i]) != NULL)
        {
            printf("I have %s and its index is %d\n", (char *)ev.attrs.elem_[i], i);
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
        const char *attribute = (char *)ev.attrs.elem_[line_it_has[i]];
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
    ptr_list new_attrs;
    get_W_rho(&new_W, &new_attrs, new_root);

    rdmat_print("new_W", new_W);
    print_list("new_attrs", new_attrs);
    RDResult my_result = get_the_result(ev->attrs, new_attrs, root, new_root);
    print_list("the deleted", my_result.the_deleted);
    print_list("the remains", my_result.the_remains); // If the remains is empty, it's better to build the policy tree from scratch rather than modify it through this method.
    element_init_GT(uev->C, pairing);
    element_init_G1(uev->C0, pairing);
    element_set(uev->C, ev->C);
    element_set(uev->C0, ev->C0);
    uev->states = initHashMap();
    uev->CX_ = initHashMap();
    for (int i = 0; i < ev->attrs.length; i++)
    {
        char *attribute = (char *)ev->attrs.elem_[i];
        // element_t *tmpC_ev = (element_t *)map_search(ev->CX_, attribute);
        // element_init_GT(uev->C, pairing);
        // element_init_G1(uev->C0, pairing);
        // element_set(uev->C, ev->C);
        // element_set(uev->C0, ev->C0);
        element_t *tmpC_uev = (element_t *)malloc(3 * sizeof(element_t));
        element_init_G1(tmpC_uev[0], pairing);
        element_init_G1(tmpC_uev[1], pairing);
        element_init_G1(tmpC_uev[2], pairing);
        // element_set(tmpC_uev[0], tmpC_ev[0]);
        // element_set(tmpC_uev[1], tmpC_ev[1]);
        // element_set(tmpC_uev[2], tmpC_ev[2]);
        element_set1(tmpC_uev[0]);
        element_set1(tmpC_uev[1]);
        element_set1(tmpC_uev[2]);
        map_insert(uev->CX_, attribute, tmpC_uev);
        map_insert(uev->states, attribute, (void *)STATE_START);
    }

    element_t tmp1, tmp2, tmp3;
    element_init_G1(tmp1, pairing);
    element_init_G1(tmp2, pairing);
    element_init_Zr(tmp3, pairing);
    element_t t_A, t_j, neg_lambda;
    element_init_Zr(t_A, pairing);
    element_init_Zr(t_j, pairing);
    element_init_Zr(neg_lambda, pairing);

    for (int i = 0; i < my_result.the_deleted.length; i++)
    {
        char *attribute = (char *)my_result.the_deleted.elem_[i];
        TreeNode *node2del = find_node_from_tree(attribute, root);
        element_t *_lambda_A = (element_t *)map_search(ip->lambda, attribute);
        if (node2del == root)
        {
            map_insert(ip->lambda, "META", (void *)_lambda_A);
            map_remove(ip->lambda, node2del->value);
            map_insert(uev->CX_, "META", map_search(uev->CX_, node2del->value));
            map_remove(uev->CX_, node2del->value);
            state_update(uev->states, attribute, LABEL_DELETE);
            element_t *tlam = map_search(ip->lambda, "META");
            element_t *tC = map_search(uev->CX_, "META");
            element_printf("NOW, the META lambda is %B, its 3 C is %B, %B, %B\n", *tlam, tC[0], tC[0], tC[0]);
            break;
        }

        element_t *tmpC = (element_t *)map_search(uev->CX_, attribute);
        element_clear(tmpC[0]);
        element_clear(tmpC[1]);
        element_clear(tmpC[2]);
        free(tmpC);
        map_remove(uev->CX_, attribute);
        state_update(uev->states, attribute, LABEL_DELETE);

        bool conn_is_and = is_and(node2del->parent->value);
        TreeNode *lvlup_node = del_from_tree(node2del); // indeed need to modeify the old tree for the later compairing with the new one.
        element_printf("\n%s's lambda is %B\n", attribute, *_lambda_A);
        printf("The level up node is %s\n", lvlup_node->value);
        if (conn_is_and)
        {
            ptr_list the_affected = get_all_under_nodes(lvlup_node);

            for (int j = 0; j < the_affected.length; j++)
            {
                TreeNode *inner_node = (TreeNode *)the_affected.elem_[j];
                element_t *_lambda_j = (element_t *)map_search(ip->lambda, inner_node->value);
                element_printf("%s's lambda is\n%B\n+%s's lambda\n%B\n", inner_node->value, *_lambda_j, attribute, *_lambda_A);
                element_add(*_lambda_j, *_lambda_j, *_lambda_A);
                element_printf("is %B\n", *_lambda_j);
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
    for (int i = 0; i < new_attrs.length; i++)
    {
        node2del = find_node_from_tree((char *)new_attrs.elem_[i], new_root);
        if (node2del != NULL && !is_attribute_in(node2del->value, my_result.the_remains))
        {
            // assert(strcmp(node2del->value, "Y") == 0);
            // find the node which is in new_attrs but not in the_remains, indicating this node was new added or first deleted and then added again.
            if (node2del == new_root)
            {
                map_insert(ip->lambda, node2del->value, map_search(ip->lambda, "META"));
                map_remove(ip->lambda, "META");
                map_insert(uev->CX_, node2del->value, map_search(uev->CX_, "META"));
                printf("%s's cx has been inserted as %p\n", node2del->value, map_search(uev->CX_, "META"));
                map_remove(uev->CX_, "META");
                state_update(uev->states, node2del->value, LABEL_ADD);

                element_t *tlam = map_search(ip->lambda, node2del->value);
                element_t *tC = map_search(uev->CX_, node2del->value);
                element_printf("NOW, the %s's lambda is %B, its 3 C is %B, %B, %B\n", node2del->value, *tlam, tC[0], tC[0], tC[0]);
            }
            else
            {
                int path = get_path(node2del);
                if (path != -1)
                {
                    int connector = is_and(node2del->parent->value) ? 1 : 0;
                    del_from_tree(node2del);

                    rd_stk_push(stack, (StkItem){.ptr = (char *)new_attrs.elem_[i], .path = path, .connector = connector});
                }
            }
        }
    }
    print_state("A", (State)map_search(uev->states, "A"));
    // So far, the new tree as well as being clipped to look just like the old one's part which shared the same sub-tree.

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
            map_insert(ip->lambda, the_new_added->value, (void *)_lambda_A);
            element_printf("Inserting %s's lambda =%B\n", the_new_added->value, *_lambda_A);

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
            element_random(*_lambda_A);
            map_insert(ip->lambda, the_new_added->value, (void *)_lambda_A);
            element_printf("Inserting %s's lambda =%B\n", the_new_added->value, *_lambda_A);

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
                printf("lambda %s is @%p\n", inner_node->value, _lambda_j);
                element_printf("=%B\n", _lambda_j);
                element_printf("%s's lambda is\n%B\n-%s\n%B\n", inner_node->value, *_lambda_j, the_new_added->value, *_lambda_A);
                element_sub(*_lambda_j, *_lambda_j, *_lambda_A);
                element_printf("is %B\n", *_lambda_j);
                element_t *tmpC_j = map_search(uev->CX_, inner_node->value);
                printf("%s's tmpC_j = %p.\n", inner_node->value, tmpC_j);
                element_neg(neg_lambda, *_lambda_A);
                element_random(t_j);
                element_pow_zn(tmp1, pk.w, neg_lambda);
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
    element_clear(t_A);
    element_clear(t_j);
    element_clear(tmp1);
    element_clear(tmp2);
    element_clear(tmp3);
    for (int i = 0; i < COUNT(UNIVERS); i++)
    {
        const char *attribute = UNIVERS[i];
        State this_state = (State)map_search(uev->states, attribute);
        if (this_state == STATE_ADD_ || this_state == STATE_MULTIPLY_)
        {
            map_update(uev->states, attribute, (void *)STATE_REPLACE);
        }
        print_state(attribute, (State)map_search(uev->states, attribute));
    }

    EV a_new_ev;
    a_new_ev.attrs = new_attrs;
    a_new_ev.W = new_W;
    ip->W = new_W;
    uev->W = new_W;
    uev->attrs = new_attrs;

    // rdmat_print("new_W", new_W);
    // print_list("new_rho", new_rho);
    // element_t *TMP_CX_Y2 = map_search(uev->CX_, "Y");
    // element_printf("================%s is added\nCY1=%B\nCY2=%B\nCY3=%B\n", "Y", TMP_CX_Y2[0], TMP_CX_Y2[1], TMP_CX_Y2[2]);

    policy_init(&a_new_ev, NULL, pk, NULL, pp_new);

    // element_init_GT(uev->C, pairing);
    // element_init_G1(uev->C0, pairing);
    // element_set(uev->C, a_new_ev.C);
    // element_set(uev->C0, a_new_ev.C0);
    // element_mul(uev->C, uev->C, a_new_ev.C);
    // element_mul(uev->C0, uev->C0, a_new_ev.C0);
    element_set(uev->C, a_new_ev.C);
    element_set(uev->C0, a_new_ev.C0);
    for (int i = 0; i < new_attrs.length; i++)
    {
        const char *attribute = (char *)new_attrs.elem_[i];
        element_t *tmpC_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *tmpC_ev_new = (element_t *)map_search(a_new_ev.CX_, attribute);

        element_mul(tmpC_uev[0], tmpC_uev[0], tmpC_ev_new[0]);
        element_mul(tmpC_uev[1], tmpC_uev[1], tmpC_ev_new[1]);
        element_mul(tmpC_uev[2], tmpC_uev[2], tmpC_ev_new[2]);
    }

    //-----

    // element_t *TMP_CX_Z = map_search(uev->CX_, "Z");
    // element_t *TMP_CX_G = map_search(uev->CX_, "G");
    // element_t *TMP_CX_Y = map_search(uev->CX_, "Y");

    // element_printf("C1Z = %B\n", TMP_CX_Z[0]);
    // element_printf("C2Z = %B\n", TMP_CX_Z[1]);
    // element_printf("C3Z = %B\n", TMP_CX_Z[2]);
    // element_printf("C1G = %B\n", TMP_CX_G[0]);
    // element_printf("C2G = %B\n", TMP_CX_G[1]);
    // element_printf("C3G = %B\n", TMP_CX_G[2]);
    // element_printf("C1Y = %B\n", TMP_CX_Y[0]);
    // element_printf("C2Y = %B\n", TMP_CX_Y[1]);
    // element_printf("C3Y = %B\n", TMP_CX_Y[2]);

    //------
}

void evidence_mod(UEV *uev, EV *ev_cur)
{
    // element_set(ev_cur->C, uev->C);
    // element_set(ev_cur->C0, uev->C0);
    element_mul(ev_cur->C, ev_cur->C, uev->C);
    element_mul(ev_cur->C0, ev_cur->C0, uev->C0);
    for (int i = 0; i < COUNT(UNIVERS); i++)
    {
        const char *attribute = UNIVERS[i];
        element_t *CX_in_uev = (element_t *)map_search(uev->CX_, attribute);
        element_t *CX_in_ev_cur = (element_t *)map_search(ev_cur->CX_, attribute);
        // printf("%s's CX_in_uev is %p, CX_in_ev_cur is %p\n", attribute, CX_in_uev, CX_in_ev_cur);
        switch ((State)map_search(uev->states, attribute))
        {
        case STATE_START:
        case STATE_MULTIPLY:
            printf("%s``````````````````````````````````MUL\n", attribute);
            element_mul(CX_in_ev_cur[0], CX_in_ev_cur[0], CX_in_uev[0]);
            element_mul(CX_in_ev_cur[1], CX_in_ev_cur[1], CX_in_uev[1]);
            element_mul(CX_in_ev_cur[2], CX_in_ev_cur[2], CX_in_uev[2]);
            // element_set(CX_in_ev_cur[0], CX_in_uev[0]);
            // element_set(CX_in_ev_cur[1], CX_in_uev[1]);
            // element_set(CX_in_ev_cur[2], CX_in_uev[2]);
            break;
        case STATE_REPLACE:
            printf("%s``````````````````````````````REPLACE\n", attribute);
            element_set(CX_in_ev_cur[0], CX_in_uev[0]);
            element_set(CX_in_ev_cur[1], CX_in_uev[1]);
            element_set(CX_in_ev_cur[2], CX_in_uev[2]);
            // element_mul(CX_in_ev_cur[0], CX_in_ev_cur[0], CX_in_uev[0]);
            // element_mul(CX_in_ev_cur[1], CX_in_ev_cur[1], CX_in_uev[1]);
            // element_mul(CX_in_ev_cur[2], CX_in_ev_cur[2], CX_in_uev[2]);
            break;
        case STATE_DELETE:
            printf("%s``````````````````````````````````DEL\n", attribute);
            map_remove(ev_cur->CX_, attribute);
            break;
        case STATE_ADD:
            printf("%s``````````````````````````````````ADD\n", attribute);
            element_t *CX2add = (element_t *)malloc(3 * sizeof(element_t));
            element_init_G1(CX2add[0], pairing);
            element_init_G1(CX2add[1], pairing);
            element_init_G1(CX2add[2], pairing);
            element_set(CX2add[0], CX_in_uev[0]);
            element_set(CX2add[1], CX_in_uev[1]);
            element_set(CX2add[2], CX_in_uev[2]);
            map_insert(ev_cur->CX_, attribute, (void *)CX2add);
            break;
        default:
            break;
        }
    }
    ev_cur->attrs = uev->attrs;
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

HashMap *get_lambda(rdmat a, ptr_list attrs, rdmat_mp b)
{
    HashMap *map = initHashMap();
    rdmat_mp c = make_rdmat_mp(a.rows, 1);
    element_t prod;
    element_init_Zr(prod, pairing);
    for (int i = 0; i < a.rows; i++)
    {
        element_set0(c.elem[i]);
        for (int j = 0; j < a.cols; j++)
        {
            element_mul_si(prod, b.elem[j], a.elem[i][j]);
            element_add(c.elem[i], c.elem[i], prod);
        }
        map_insert(map, (char *)attrs.elem_[i], c.elem[i]);
    }
    element_clear(prod);
    return map;
}

RDResult get_the_result(ptr_list attrs1, ptr_list attrs2, TreeNode *root1, TreeNode *root2)
{
    RDResult my_result = {
        .the_deleted = make_ptr_list(0),
        .the_remains = make_ptr_list(0),
    };
    for (int i = 0; i < attrs1.length; i++)
    {
        char *this_attribute = (char *)attrs1.elem_[i];
        if (!is_attribute_in(this_attribute, attrs2) || !is_same_path(find_node_from_tree(this_attribute, root2), find_node_from_tree(this_attribute, root1)))
        {
            my_result.the_deleted = add_to_list(my_result.the_deleted, strdup(this_attribute));
        }
    }
    for (int i = 0; i < attrs2.length; i++)
    {
        char *this_attribute = attrs2.elem_[i];
        if (is_attribute_in(this_attribute, attrs1))
        {
            if (is_same_path(find_node_from_tree(this_attribute, root2), find_node_from_tree(this_attribute, root1)))
            {
                my_result.the_remains = add_to_list(my_result.the_remains, strdup(this_attribute));
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
