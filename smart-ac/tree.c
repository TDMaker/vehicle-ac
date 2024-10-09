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
