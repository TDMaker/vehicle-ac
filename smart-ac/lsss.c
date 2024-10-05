#include "lsss.h"
int cols = 1;
int rows = 0;
int *tmp_elem;
char **tmp_rho;

void set_childrens_vec(TreeNode *node, void *data)
{
    // printf("pointer is %p, value is %s, cols is %d.\n", node, node->value, cols);
    if (strcmp(node->value, "&&") == 0)
    {
        cols++;
        node->left->vec = make_rdvec();
        node->right->vec = make_rdvec();
        node->right->vec.length = node->left->vec.length = cols;
        memcpy(node->right->vec.data, node->vec.data, sizeof(int) * cols);
        node->right->vec.data[cols - 1] = 1;
        node->left->vec.data[cols - 1] = -1;
    }
    else if (strcmp(node->value, "||") == 0)
    {
        node->left->vec = make_rdvec();
        node->right->vec = make_rdvec();
        int parent_length = node->vec.length;
        node->right->vec.length = node->left->vec.length = parent_length;
        memcpy(node->left->vec.data, node->vec.data, sizeof(int) * parent_length);
        memcpy(node->right->vec.data, node->vec.data, sizeof(int) * parent_length);
    }
    else
    {
        rows++;
    }
}

void display(TreeNode *node, void *data)
{
    // printf("the node's addr is %p, value is %s\n", node, node->value);
    if (strcmp(node->value, "||") && strcmp(node->value, "&&"))
    {
        printf("%s: [", node->value);
        for (int i = 0; i < node->vec.length; i++)
        {
            printf("%2d ", node->vec.data[i]);
        }
        puts("]");
    }
}

TreeNode *get_complete_tree(char *input)
{
    TreeNode *root = get_root(input);
    root->vec = make_rdvec();
    root->vec.data[0] = 1;
    root->vec.length = 1;
    root->parent = NULL;
    breadth_first_traversal(root, set_childrens_vec, NULL);
    return root;
}

void get_W_rho(rdmat2 *W, char ***rho, TreeNode *root)
{
    W->cols = cols;
    W->rows = 0;
    W->elem = (int **)malloc(sizeof(char *) * rows);
    for (int i = 0; i < rows; i++)
    {
        W->elem[i] = (int *)calloc(sizeof(int), cols);
    }
    tmp_rho = (char **)malloc(sizeof(char *) * rows);
    breadth_first_traversal(root, rdmat_row_concat, W);
    *rho = tmp_rho;
    rdmat2_print("W", *W);
}

void rdmat_row_concat(TreeNode *node, void *data)
{
    rdmat2 *W = (rdmat2 *)data;
    if (strcmp(node->value, "||") && strcmp(node->value, "&&"))
    {
        memcpy(W->elem[W->rows], node->vec.data, node->vec.length * sizeof(int));
        memset(W->elem[W->rows] + node->vec.length, 0, (W->cols - node->vec.length) * sizeof(int)); // padding 0s
        tmp_rho[W->rows] = strdup(node->value);
        W->rows++;
    }
}

void rd_free_tree(TreeNode *tree)
{
    free_tree(tree);
}
