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
