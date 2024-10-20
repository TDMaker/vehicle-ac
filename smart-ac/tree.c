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
        puts("The parent of this node is NULL!");
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

TreeNode *find_in(TreeNode *a, ptr_list b)
{
    for (int i = 0; i < b.length; i++)
    {
        if (strcmp(((TreeNode *)b.elem_[i])->value, a->value) == 0)
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
    printf("processing %s\n", attribute);
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
    if (is_or(parent->value))
    {
        // element_clear(ev->C1_[index]);
        // element_clear(ev->C2_[index]);
        // element_clear(ev->C3_[index]);
    }
    else if (is_and(parent->value))
    {
        for (int i = 0; i < sibling->vec.length; i++)
        {
            sibling->vec.data[i] += this_node->vec.data[i];
        }
        // sibling->vec.length--;
    }
    else
    {
        fprintf(stderr, "ERROR! The parent of the handling this_node is not a connector! %s\n", parent->value);
        exit(-1);
    }

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