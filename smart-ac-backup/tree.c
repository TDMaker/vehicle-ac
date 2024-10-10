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

Result get_result(ptr_list rho1, ptr_list rho2)
{
    Result result = {
        .deleted_attributes_connected_by_or = make_ptr_list(0),
        .deleted_attributes_connected_by_and = make_ptr_list(0),
        .added_attributes_connected_by_or = make_ptr_list(0),
        .added_attributes_connected_by_and = make_ptr_list(0),
        .replaced_attributes_connected_by_or = make_ptr_list(0),
        .replaced_attributes_connected_by_and = make_ptr_list(0),
    };

    for (int i = 0; i < rho1.length; i++)
    {
        if (find_in(rho1.elem_[i], rho2) == NULL)
        {
            if (strcmp(((TreeNode *)rho1.elem_[i])->parent->value, "||") == 0)
            {
                result.deleted_attributes_connected_by_or = add_to_list(result.deleted_attributes_connected_by_or, rho1.elem_[i]);
            }
            else
            {
                result.deleted_attributes_connected_by_and = add_to_list(result.deleted_attributes_connected_by_and, rho1.elem_[i]);
            }
        }
    }
    TreeNode *candidate = NULL;
    for (int i = 0; i < rho2.length; i++)
    {
        if ((candidate = find_in(rho2.elem_[i], rho1)) == NULL)
        {
            if (strcmp(((TreeNode *)rho2.elem_[i])->parent->value, "||") == 0)
            {
                result.added_attributes_connected_by_or = add_to_list(result.added_attributes_connected_by_or, rho2.elem_[i]);
            }
            else
            {
                result.added_attributes_connected_by_and = add_to_list(result.added_attributes_connected_by_and, rho2.elem_[i]);
            }
        }
        else
        {
            if (!is_same_path(rho2.elem_[i], candidate))
            {
                if (strcmp(((TreeNode *)rho2.elem_[i])->parent->value, "||") == 0)
                {
                    result.replaced_attributes_connected_by_or = add_to_list(result.replaced_attributes_connected_by_or, rho2.elem_[i]);
                }
                else
                {
                    result.replaced_attributes_connected_by_and = add_to_list(result.replaced_attributes_connected_by_and, rho2.elem_[i]);
                }
            }
        }
    }
    print_list("deleted_attributes_connected_by_or", result.deleted_attributes_connected_by_or);
    print_list("deleted_attributes_connected_by_and", result.deleted_attributes_connected_by_and);
    print_list("added_attributes_connected_by_or", result.added_attributes_connected_by_or);
    print_list("added_attributes_connected_by_or", result.added_attributes_connected_by_and);
    print_list("replaced_attributes_connected_by_and", result.replaced_attributes_connected_by_and);
    print_list("replaced_attributes_connected_by_or", result.replaced_attributes_connected_by_or);
    return result;
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
