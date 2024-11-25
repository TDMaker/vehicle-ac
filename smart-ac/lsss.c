#include <include/lsss.h>
int cols = 1;
int rows = 0;
char **tmp_attrs;

void set_childrens_vec(TreeNode *node, void **food)
{
    if (is_and(node->value))
    {
        cols++;
        if (node->left->vec.length == 0)
        {
            node->left->vec = make_rdvec();
        }
        else
        {
            memset(node->left->vec.data, 0, CAPACITY);
        }
        node->left->vec.length = cols;
        if (node->right->vec.length == 0)
        {
            node->right->vec = make_rdvec();
            node->right->vec.length = cols;
        }
        else
        {
            memset(node->right->vec.data, 0, CAPACITY);
        }
        node->right->vec.length = cols;
        memcpy(node->right->vec.data, node->vec.data, sizeof(int) * cols);
        node->right->vec.data[cols - 1] = 1;
        node->left->vec.data[cols - 1] = -1;
    }
    else if (is_or(node->value))
    {
        if (node->left->vec.length == 0)
        {
            node->left->vec = make_rdvec();
        }
        else
        {
            memset(node->left->vec.data, 0, CAPACITY);
        }
        node->left->vec.length = node->vec.length;
        if (node->right->vec.length == 0)
        {
            node->right->vec = make_rdvec();
        }
        else
        {
            memset(node->left->vec.data, 0, CAPACITY);
        }
        node->right->vec.length = node->vec.length;
        memcpy(node->left->vec.data, node->vec.data, sizeof(int) * node->vec.length);
        memcpy(node->right->vec.data, node->vec.data, sizeof(int) * node->vec.length);
    }
    else
    {

        rows++;
    }
}

void display(TreeNode *node, void **food)
{
    // printf("the node's addr is %p, value is %s\n", node, node->value);
    if (!is_connector(node->value))
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
    init_vec(root);
    return root;
}

void init_vec(TreeNode *root)
{
    cols = 1;
    rows = 0;
    breadth_first_traversal(root, set_childrens_vec, NULL);
}

void get_W_rho(rdmat *W, ptr_list *edge, TreeNode *root)
{
    W->cols = cols;
    W->rows = 0;
    W->elem = (int **)malloc(sizeof(char *) * rows);
    for (int i = 0; i < rows; i++)
    {
        W->elem[i] = (int *)calloc(sizeof(int), cols);
    }

    tmp_attrs = (char **)malloc(sizeof(char *) * rows);
    breadth_first_traversal(root, rdmat_row_concat, (void **)&W);
    edge->elem_ = (void **)tmp_attrs;
    edge->length = W->rows;
    edge->capacity = W->rows;
    tmp_attrs = NULL;
}

void rdmat_row_concat(TreeNode *node, void **food)
{
    rdmat *W = (rdmat *)*food; // NOTE
    if (!is_connector(node->value))
    {
        memcpy(W->elem[W->rows], node->vec.data, node->vec.length * sizeof(int));
        memset(W->elem[W->rows] + node->vec.length, 0, (W->cols - node->vec.length) * sizeof(int)); // padding 0s
        tmp_attrs[W->rows] = strdup(node->value);
        W->rows++;
    }
}

void rd_free_tree(TreeNode *tree)
{
    free_tree(tree);
}
