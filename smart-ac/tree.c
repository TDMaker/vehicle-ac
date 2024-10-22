#include "tree.h"

// ptr_list get_diff(ptr_list rho1, ptr_list rho2)
// {
//     ptr_list result = make_ptr_list(rho1.length);
//     for (int i = 0; i < rho1.length; i++)
//     {
//         int has = 0;
//         for (int j = 0; j < rho2.length; j++)
//         {
//             if (strcmp(((TreeNode *)rho1.elem_[i])->value, ((TreeNode *)rho2.elem_[j])->value) == 0)
//             {
//                 has = 1;
//                 break;
//             }
//         }
//         if (has == 0)
//         {
//             result.elem_[result.length] = rho1.elem_[i];
//             printf("different element %s @ %p\n", ((TreeNode *)rho1.elem_[i])->value, rho1.elem_[i]);
//             result.length++;
//         }
//     }
//     return result;
// }

TreeNode *get_sibling(TreeNode *node)
{
    TreeNode *parent = node->parent;
    if (parent == NULL)
    {
        fprintf(stderr, "The parent of %s is NULL!\n", node->value);
        return NULL;
    }
    else
    {
        if (parent->left == node && parent->right != node)
        {
            return parent->right;
        }
        else if (parent->left != node && parent->right == node)
        {
            return parent->left;
        }
        else
        {
            puts("Paternity gone wrong!");
            return NULL;
        }
    }
}

TreeNode *find_node_in(TreeNode *a, ptr_list b)
{
    for (int i = 0; i < b.length; i++)
    {
        if (strcmp(((TreeNode *)b.elem_[i])->value, a->value) == 0)
            return (TreeNode *)b.elem_[i];
    }
    return NULL;
}
TreeNode *find_attribute_in(const char *a, ptr_list b)
{
    for (int i = 0; i < b.length; i++)
    {
        if (strcmp(((TreeNode *)b.elem_[i])->value, a) == 0)
            return (TreeNode *)b.elem_[i];
    }
    return NULL;
}
bool is_same_path(TreeNode *a, TreeNode *b)
{
    if (a == NULL && b == NULL)
    {
        return true;
    }
    else if (a == NULL || b == NULL)
    {
        return false;
    }
    else
    {
        return strcmp(a->value, b->value) == 0 && is_same_path(a->parent, b->parent);
    }
}

TreeNode *find_node_from_tree(const char *attribute, TreeNode *node)
{
    if (is_connector(node->value))
    {
        TreeNode *left = find_node_from_tree(attribute, node->left);
        if (left == NULL)
        {
            return find_node_from_tree(attribute, node->right);
        }
        else
        {
            return left;
        }
    }
    else
    {
        if (strcmp(attribute, node->value) == 0)
        {
            return node;
        }
        else
        {
            return NULL;
        }
    }
}

void print_list(const char *name, ptr_list a)
{
    printf("Elemenets in %s is:\n", name);
    for (int i = 0; i < a.length; i++)
    {
        printf("%s ", ((TreeNode *)a.elem_[i])->value);
    }
    puts("\n===============");
}

int is_same_tree(TreeNode *a, TreeNode *b)
{
    if (a == NULL && b == NULL)
        return 1;
    else if ((a == NULL) || (b = NULL))
        return 0;
    else
        return (!strcmp(a->value, b->value) == 0) && ((is_same_tree(a->left, b->left) && is_same_tree(a->right, b->right)) || (is_same_tree(a->left, b->right) && is_same_tree(a->right, b->left)));
}
TreeNode *del_from_tree(ptr_list *rho, const char *attribute)
{
    printf("deleting %s\n", attribute);
    TreeNode *this_node = NULL;
    int index = -1;
    for (int i = 0; i < rho->length; i++)
    {
        if (rho->elem_[i] != NULL && strcmp(attribute, ((TreeNode *)rho->elem_[i])->value) == 0)
        {
            this_node = (TreeNode *)rho->elem_[i];
            index = i;
            break;
        }
    }
    if (this_node == NULL)
    {
        fprintf(stderr, "The attribute's corresponding node is not found!\n");
        exit(-1);
    }
    TreeNode *sibling = get_sibling(this_node);
    TreeNode *parent = this_node->parent;
    // if (is_or(parent->value))
    // {
    //     // element_clear(ev->C1_[index]);
    //     // element_clear(ev->C2_[index]);
    //     // element_clear(ev->C3_[index]);
    // }
    // else if (is_and(parent->value))
    // {
    //     for (int i = 0; i < sibling->vec.length; i++)
    //     {
    //         sibling->vec.data[i] += this_node->vec.data[i];
    //     }
    //     // sibling->vec.length--;
    // }
    // else
    // {
    //     fprintf(stderr, "ERROR! The parent of the handling this_node is not a connector! %s\n", parent->value);
    //     exit(-1);
    // }
    strcpy(parent->value, sibling->value);
    free_rdvec(parent->vec);
    parent->vec = sibling->vec;
    parent->left = sibling->left;
    parent->right = sibling->right;
    if (is_connector(sibling->value))
    {
        sibling->left->parent = parent;
        sibling->right->parent = parent;
        rho->elem_[index] = NULL;
    }
    else
    {
        rho->elem_[index] = parent;
        printf("%s is replacing\n", parent->value);
    }
    // update the address recorded in the old rho.
    for (int i = 0; i < rho->length; i++)
    {
        if (sibling == rho->elem_[i] && !is_connector(sibling->value))
        {
            rho->elem_[i] = NULL;
            break;
        }
    }
    free(sibling);
    free_rdvec(this_node->vec);
    free(this_node);

    return parent;
}

int del_from_tree2(TreeNode *node2del)
{
    TreeNode *sibling = get_sibling(node2del);
    TreeNode *parent = node2del->parent;
    int connector = -1;
    if (is_or(parent->value))
    {
        connector = 0;
    }
    else if (is_and(parent->value))
    {
        connector = 1;
    }
    strcpy(parent->value, sibling->value);
    parent->left = sibling->left;
    parent->right = sibling->right;
    if (parent->left)
    {
        parent->left->parent = parent;
    }
    if (parent->right)
    {
        parent->right->parent = parent;
    }
    free(sibling);
    free(node2del);

    return connector;
}

void fill_the_tree(ptr_list *new_rho, const char *attribute, TreeNode *this_node)
{
    TreeNode *sibling = get_sibling(this_node);
    if (is_connector(sibling->value))
    {
        // fill_the_tree(sibling->left);
        // fill_the_tree(sibling->right);
    }
    else
    {
        // if sibling->value not in added_list
        // add sibling to the tree and the list
    }
    // TreeNode *this_node;
    // ptr_list nodes_unders = get_all_under_nodes(get_sibling(this_node));
    // fill_the_tree(get_sibling(this_node->parent));
}

void _find_that_kid(TreeNode *node, void **food)
{
    if (!is_connector(node->value))
    {
        *(ptr_list *)food = add_to_list(*(ptr_list *)food, node);
    }
}
ptr_list get_all_under_nodes(TreeNode *_node)
{
    ptr_list list = make_ptr_list(0);
    if (is_connector(_node->value))
    {
        breadth_first_traversal(_node, _find_that_kid, (void **)&list);
    }
    else
    {
        list = add_to_list(list, _node->value);
    }
    return list;
}

TreeNode *add_to_tree(ptr_list rho, TreeNode *orign_node, TreeNode *sibling_in_new_rho)
{
    TreeNode *self_spawn = (TreeNode *)malloc(sizeof(TreeNode));
    TreeNode *sibling_append = (TreeNode *)malloc(sizeof(TreeNode));
    self_spawn->right = self_spawn->left = NULL;
    sibling_append->right = sibling_append->left = NULL;
    strcpy(self_spawn->value, orign_node->value);
    strcpy(sibling_append->value, sibling_in_new_rho->value);
    strcpy(orign_node->value, sibling_in_new_rho->parent->value);
    orign_node->left = self_spawn;
    orign_node->right = sibling_append;
    self_spawn->parent = orign_node;
    sibling_append->parent = orign_node;
    print_node(orign_node);
    print_node(self_spawn);
    print_node(sibling_append);
    if (!is_connector(self_spawn->value))
    {
        for (int i = 0; i < rho.length; i++)
        {
            if (rho.elem_[i] == orign_node)
            {
                rho.elem_[i] = self_spawn;
                break;
            }
        }
    }
    return sibling_append;
}

TreeNode* add_to_tree2(TreeNode *root, int path, int connector, const char *attribute)
{
    int direction = 0;
    TreeNode *end_node = root;
    printf("Adding %s\n", attribute);
    while (path > 3)
    {
        direction = path & 1;
        end_node = direction == 0 ? end_node->left : end_node->right;
        path >>= 1;
    }
    direction = path & 1;
    TreeNode *kid1 = (TreeNode *)malloc(sizeof(TreeNode));
    TreeNode *kid2 = (TreeNode *)malloc(sizeof(TreeNode));
    kid1->left = end_node->left;
    kid1->right = end_node->right;
    kid2->left = NULL;
    kid2->right = NULL;
    if (direction == 0)
    {
        end_node->right = kid1;
        end_node->left = kid2;
    }
    else
    {
        end_node->left = kid1;
        end_node->right = kid2;
    }
    strcpy(kid1->value, end_node->value);
    strcpy(kid2->value, attribute);
    strcpy(end_node->value, connector == 0 ? "||" : "&&");
    kid1->parent = end_node;
    kid2->parent = end_node;
    return kid2;

}

/*
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
}*/

TreeNode *get_node_in_another_tree(TreeNode *leaf_node1, ptr_list new_rho)
{
    TreeNode *leaf_node2 = find_attribute_in(leaf_node1->value, new_rho);
    while ((leaf_node1 = leaf_node1->parent->parent) != NULL)
    {
        leaf_node1 = leaf_node1->parent;
        leaf_node2 = leaf_node2->parent;
    }
    return leaf_node2->parent;
}
void print_tree(TreeNode *node)
{
    printf("I am %s, my left is %p, right is %p\n", node->value, node->left, node->right);
    if (node != NULL)
    {
        if (is_connector(node->value))
        {
            printf("left->");
            print_tree(node->left);
            printf("right->");
            print_tree(node->right);
        }
        else
        {
            puts(node->value);
        }
    }
}
// void branch_it(ptr_list rho, TreeNode *target_node, TreeNode *template_node, element_t *lambda)
// {

//     if (!is_connector(template_node->value))
//     {
//         // it's a attr
//         return;
//     }
//     TreeNode *left_node = (TreeNode *)malloc(sizeof(TreeNode));
//     TreeNode *right_node = (TreeNode *)malloc(sizeof(TreeNode));
//     left_node->left = left_node->right = right_node->left = right_node->right = NULL;
//     left_node->parent = target_node;
//     right_node->parent = target_node;
//     strcpy(left_node->value, template_node->left->value);
//     strcpy(right_node->value, template_node->right->value);

//     if (is_or(template_node->value))
//     {
//         branch_it(rho, left_node, template_node->left, lambda);
//         branch_it(rho, right_node, template_node->right, lambda);
//     }
//     else if (is_and(template_node->value))
//     {
//         // branch_it(node->left);
//         // branch_it(node->right);
//     }
// }

TreeNode *get_top(TreeNode *a)
{
    while (a->parent != NULL)
        a = a->parent;
    return a;
}

int get_path(TreeNode *node)
{
    if (node == NULL)
        return -1;
    int _path = 1;
    while (node->parent != NULL)
    {
        _path <<= 1;
        if (node->parent->right == node)
        {
            _path |= 1;
        }
        node = node->parent;
    }
    return _path;
}
