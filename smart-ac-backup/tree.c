#include "tree.h"

ptr_list get_diff(ptr_list rho1, ptr_list rho2)
{
    ptr_list result = make_ptr_list(rho1.length);
    for (int i = 0; i < rho1.length; i++)
    {
        int has = 0;
        for (int j = 0; j < rho2.length; j++)
        {
            if (strcmp(((TreeNode *)rho1.elem_[i])->value, ((TreeNode *)rho2.elem_[j])->value) == 0)
            {
                has = 1;
                break;
            }
        }
        if (has == 0)
        {
            result.elem_[result.length] = rho1.elem_[i];
            printf("different element %s @ %p\n", ((TreeNode *)rho1.elem_[i])->value, rho1.elem_[i]);
            result.length++;
        }
    }
    return result;
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
        return is_same_path(a->parent, b->parent);
    }
}

ptr_list add_to_list(ptr_list a, TreeNode *b)
{
    if (a.length >= a.capacity)
    {
        ptr_list tmp = make_ptr_list(a.capacity * 2);
        memcpy(tmp.elem_, a.elem_, sizeof(TreeNode *) * a.length);
        tmp.length = a.length;
        tmp.elem_[tmp.length++] = b;
        free(a.elem_);
        return tmp;
    }
    else
    {
        a.elem_[a.length++] = b;
        return a;
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
