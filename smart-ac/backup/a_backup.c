   // element_t *tmp;
    // for (int i = 0; i < result.deleted_attributes_connected_by_or.length; i++)
    // {
    //     const char *attribute = ((TreeNode *)result.deleted_attributes_connected_by_or.elem_[i])->value;
    //     tmp = map_search(ip->lambda, attribute);
    //     element_clear(*tmp);
    //     map_remove(ip->lambda, attribute);
    //     tmp = map_search(ev->CX_, attribute);
    //     element_clear(*tmp++);
    //     element_clear(*tmp++);
    //     element_clear(*tmp);
    //     map_remove(ev->CX_, attribute);
    // }


    // int row_removed = -1;
    // for (int i = 0; i < ev->rho.length; i++)
    // {
    //     int has = 0;
    //     for (int j = 0; j < new_rho.length; j++)
    //     {
    //         if (ev->rho.elem_[i] == new_rho.elem_[j])
    //         {
    //             printf("ev->rho.node_[%d].val = %s\n", i, ((TreeNode **)ev->rho.elem_)[i]->value);
    //             has = 1;
    //             break;
    //         }
    //     }
    //     if (has == 0)
    //     {
    //         row_removed = i;
    //         break;
    //     }
    // }

    // printf("The %dth node has been removed.\n", row_removed);

    // int **tmp_W = (int **)malloc(sizeof(int *) * ev->W.rows);
    // for (int i = 0; i < ev->W.rows; i++)
    // {
    //     tmp_W[i] = NULL;
    // }

    // for (int i = 0; i < ev->rho.length; i++)
    // {
    //     for (int j = 0; j < new_rho.length; j++)
    //     {
    //         if (ev->rho.elem_[i] == new_rho.elem_[j])
    //         {
    //             tmp_W[i] = ev->W.elem[j];
    //             break;
    //         }
    //     }
    // }
    // for (int i = 0; i < new_rho.length; i++)
    // {
    //     if (tmp_W[i] == NULL)
    //     {
    //         for (int j = i; j < new_rho.length; j++)
    //         {
    //             tmp_W[j] = tmp_W[j + 1];
    //         }
    //     }
    // }
    // free(ev->W.elem);
    // ev->W.elem = tmp_W;
    // rdmat_print("new W", ev->W);

    // element_t **new_C1_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    // element_t **new_C2_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    // element_t **new_C3_ = (element_t **)malloc(sizeof(element_t *) * new_rho.length);
    // element_t **new_lambda = (element_t **)malloc(sizeof(element_t *) * new_rho.length);

    // for (int i = 0, j = 0; i < ev->rho.length; i++, j++)
    // {
    //     if (i == row_removed)
    //     {
    //         element_free(*ev->C1_[i]);
    //         element_free(*ev->C2_[i]);
    //         element_free(*ev->C3_[i]);
    //         element_free(*ip->lambda[i]);
    //         j--;
    //         continue;
    //     }
    //     new_C1_[j] = ev->C1_[i];
    //     new_C2_[j] = ev->C2_[i];
    //     new_C3_[j] = ev->C3_[i];
    //     new_lambda[j] = ip->lambda[i];
    // }

    // ev->C1_ = new_C1_;
    // ev->C2_ = new_C2_;
    // ev->C3_ = new_C3_;
    // ip->lambda = new_lambda;
// -----------------------------------------------------------------
    // add_or(root, "F");
    // TODO:
    // get_W_rho();
    // breadth_first_traversal(root, display, NULL);

            // element_t *lambda = (element_t *)map_search(ip->lambda, attribute);
        // if (lambda == NULL)
        // {
        //     printf("%s's lambda is NULL", attribute);
        // }
        // else
        // {
        //     element_printf("%s's lambda is %B\n", attribute, *lambda);
        // }
        // element_t *C = (element_t *)map_search(uev->CX_, attribute);
        // if (C != NULL)
        //     element_printf("%s's\nC1 is\n%B\nC2 is\n%BC3 is\n%B\n", attribute, *(element_t *)C[0], *(element_t *)C[1], *(element_t *)C[2]);

// ----------------------------------------------

// void _find_that_kid(TreeNode *node, void **food)
// {
//     if (find_in(node, *(ptr_list *)food[0]))
//     {
//         *(ptr_list *)food[1] = add_to_list(*(ptr_list *)food[1], node);
//     }
// }
// ptr_list get_the_affected(TreeNode *_node, ptr_list _remains)
// {
//     TreeNode *sibling = get_sibling(_node);
//     ptr_list list = make_ptr_list(0);
//     if (strcmp(sibling->value, "&&") == 0 || strcmp(sibling->value, "||") == 0)
//     {
//         void **my_food = (void **)malloc(2 * sizeof(void *));
//         my_food[0] = &_remains;
//         my_food[1] = &list;
//         breadth_first_traversal(sibling, _find_that_kid, my_food);
//     }
//     else
//     {
//         list = add_to_list(list, sibling->value);
//     }
//     return list;
// }