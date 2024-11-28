#include <tree.h>

TreeNode *get_sibling(TreeNode *node)
{
    TreeNode *parent = node->parent;
    if (parent == NULL)
    {
        fprintf(stderr, "The parent of %s is NULL!\n", node->value);
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

bool is_attribute_in(const char *a, ptr_list b)
{
    for (int i = 0; i < b.length; i++)
    {
        if (strcmp((char *)b.elem_[i], a) == 0)
            return true;
    }
    return false;
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

TreeNode *find_node_from_tree(const char *attribute, TreeNode *node)
{
    if (is_connector(node->value))
    {
        TreeNode *left = find_node_from_tree(attribute, node->left);
        if (left == NULL)
        {
            return find_node_from_tree(attribute, node->right);
        }
        else
        {
            return left;
        }
    }
    else
    {
        if (strcmp(attribute, node->value) == 0)
        {
            return node;
        }
        else
        {
            return NULL;
        }
    }
}

void print_list(const char *name, ptr_list a)
{
    printf("Elemenets in %s is:\n", name);
    for (int i = 0; i < a.length; i++)
    {
        printf("%s ", (char *)a.elem_[i]);
    }
    puts("\n===============");
}

int is_same_tree(TreeNode *a, TreeNode *b)
{
    if (a == NULL && b == NULL)
        return 1;
    else if ((a == NULL) || (b == NULL))
        return 0;
    else
        return (strcmp(a->value, b->value) == 0) && ((is_same_tree(a->left, b->left) && is_same_tree(a->right, b->right)) || (is_same_tree(a->left, b->right) && is_same_tree(a->right, b->left)));
}

TreeNode *del_from_tree(TreeNode *node2del)
{
    TreeNode *sibling = get_sibling(node2del);
    TreeNode *parent = node2del->parent;
    strcpy(parent->value, sibling->value);
    parent->left = sibling->left;
    parent->right = sibling->right;
    if (parent->left)
    {
        parent->left->parent = parent;
    }
    if (parent->right)
    {
        parent->right->parent = parent;
    }
    free(sibling);
    free(node2del);

    return parent;
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

TreeNode *add_to_tree(TreeNode *root, int path, int connector, const char *attribute)
{
    int direction = 0;
    TreeNode *end_node = root;
    printf("Adding %s\n", attribute);
    while (path > 3)
    {
        direction = path & 1;
        end_node = direction == 0 ? end_node->left : end_node->right;
        path >>= 1;
    }
    direction = path & 1;
    TreeNode *kid1 = (TreeNode *)malloc(sizeof(TreeNode));
    TreeNode *kid2 = (TreeNode *)malloc(sizeof(TreeNode));
    kid1->left = end_node->left;
    kid1->right = end_node->right;
    kid2->left = NULL;
    kid2->right = NULL;
    if (direction == 0)
    {
        end_node->right = kid1;
        end_node->left = kid2;
    }
    else
    {
        end_node->left = kid1;
        end_node->right = kid2;
    }
    strcpy(kid1->value, end_node->value);
    strcpy(kid2->value, attribute);
    strcpy(end_node->value, connector == 0 ? "||" : "&&");
    kid1->parent = end_node;
    kid2->parent = end_node;
    return kid2;
}

void print_tree(TreeNode *node)
{
    // printf("I am %s, my left is %p, right is %p\n", node->value, node->left, node->right);
    if (node != NULL)
    {
        if (is_connector(node->value))
        {
            printf("%s -> left:", node->value);
            print_tree(node->left);
            printf("%s -> right:", node->value);
            print_tree(node->right);
        }
        else
        {
            printf("%s\n", node->value);
        }
    }
}

TreeNode *get_top(TreeNode *a)
{
    while (a->parent != NULL)
        a = a->parent;
    return a;
}

int get_path(TreeNode *node)
{
    if (node == NULL)
        return -1;
    int _path = 1;
    while (node->parent != NULL)
    {
        _path <<= 1;
        if (node->parent->right == node)
        {
            _path |= 1;
        }
        node = node->parent;
    }
    return _path;
}
