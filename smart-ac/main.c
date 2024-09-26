#include "lsss.h"

// 主函数
int main()
{
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";

    TreeNode *root = get_complete_tree(input);
    breadth_first_traversal(root, display);
    return 0;
}