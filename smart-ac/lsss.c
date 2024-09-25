#include "lsss.h"
extern int counter;

void set_childrens_vec(TreeNode *node, int)
{
    if (strcmp(node->value, "&&") == 0)
    {
        counter++;
        if (node->left->vec == NULL)
            node->left->vec = make_rdvec();
        if (node->right->vec == NULL)
            node->right->vec = make_rdvec();
        (*node->right->vec).length = (*node->left->vec).length = counter;
        memset(node->left->vec->data, 0, sizeof(int) * counter);
        memset(node->right->vec->data, 0, sizeof(int) * counter);
        memcpy(node->right->vec->data, node->vec->data, sizeof(int) * counter);
        node->right->vec->data[counter - 1] = 1;
        node->left->vec->data[counter - 1] = -1;
    }
    else if (strcmp(node->value, "||") == 0)
    {
        if (node->left->vec == NULL)
            node->left->vec = make_rdvec();
        if (node->right->vec == NULL)
            node->right->vec = make_rdvec();
        int parent_length = (*node->vec).length;
        (*node->right->vec).length = (*node->left->vec).length = parent_length;
        memcpy(node->left->vec->data, node->vec->data, sizeof(int) * parent_length);
        memcpy(node->right->vec->data, node->vec->data, sizeof(int) * parent_length);
    }
}
void pad_0s(TreeNode *node, int length_set)
{
    int gap = length_set - (*node->vec).length;
    for (int i = (*node->vec).length; i < length_set; i++)
    {
        node->vec->data[i] = 0;
    }
    (*node->vec).length = length_set;
}

TreeNode *get_complete_tree(char *input)
{
    TreeNode *root = get_root(input);
    root->vec = make_rdvec();
    root->vec->data[0] = 1;
    (*root->vec).length = 1;
    breadth_first_traversal(root, set_childrens_vec, FILLING);
    breadth_first_traversal(root, pad_0s, PADDING);
    return root;
}