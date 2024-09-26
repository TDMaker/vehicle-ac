#include "lsss.h"
extern int counter;

void set_childrens_vec(TreeNode *node)
{
    // printf("pointer is %p, value is %s, counter is %d.\n", node, node->value, counter);
    if (strcmp(node->value, "&&") == 0)
    {
        counter++;
        node->left->vec = make_rdvec();
        node->right->vec = make_rdvec();
        (*node->right->vec).length = (*node->left->vec).length = counter;
        memcpy(node->right->vec->data, node->vec->data, sizeof(int) * counter);
        node->right->vec->data[counter - 1] = 1;
        node->left->vec->data[counter - 1] = -1;
    }
    else if (strcmp(node->value, "||") == 0)
    {
        node->left->vec = make_rdvec();
        node->right->vec = make_rdvec();
        int parent_length = (*node->vec).length;
        (*node->right->vec).length = (*node->left->vec).length = parent_length;
        memcpy(node->left->vec->data, node->vec->data, sizeof(int) * parent_length);
        memcpy(node->right->vec->data, node->vec->data, sizeof(int) * parent_length);
    }
}
void pad_0s(TreeNode *node)
{
    int length_set = counter;
    if (strcmp(node->value, "||") && strcmp(node->value, "&&"))
    {
        int gap = length_set - (*node->vec).length;
        for (int i = (*node->vec).length; i < length_set; i++)
        {
            node->vec->data[i] = 0;
        }
        (*node->vec).length = length_set;
    }
    // rows++;
}

void display(TreeNode *node)
{
    // printf("the node's addr is %p, value is %s\n", node, node->value);
    if (strcmp(node->value, "||") && strcmp(node->value, "&&"))
    {
        printf("%s: [", node->value);
        for (int i = 0; i < (*node->vec).length; i++)
        {
            printf("%2d ", node->vec->data[i]);
        }
        puts("]");
    }
}

TreeNode *get_complete_tree(char *input)
{
    TreeNode *root = get_root(input);
    root->vec = make_rdvec();
    root->vec->data[0] = 1;
    (*root->vec).length = 1;
    breadth_first_traversal(root, set_childrens_vec);
    breadth_first_traversal(root, pad_0s);
    return root;
}

// void get_W(TreeNode *root)
// {
//     W = (rdmat_f *)malloc(sizeof(rdmat_f));
//     printf("rows = %d, cols = %d\n", rows, counter);
//     (*W).cols = counter;
//     (*W).rows = 0;
//     (*W).elem = (float *)calloc(sizeof(float), rows * counter);
//     // breadth_first_traversal(root, rdmat_row_concat);
// }

// void rdmat_row_concat(TreeNode *node)
// {
//     // memcpy((*W).elem + (*W).rows * (*W).cols, node->vec->data, (*node->vec).length);
//     for (int i = 0; i < (*node->vec).length; i++)
//     {
//         W->elem[(*W).rows * (*W).cols + i] = (float)node->vec->data[i];
//     }
//     (*W).rows++;
// }
