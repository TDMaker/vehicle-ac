#ifndef __LSSS_H__
#define __LSSS_H__
#include "act_visitor.h"
void set_childrens_vec(TreeNode *node);
void pad_0s(TreeNode *node);
void display(TreeNode *node);
TreeNode *get_complete_tree(char *input);

typedef struct
{
    int rows;
    int cols;
    int *elem;
} rdmat;

void rdmat_row_concat(TreeNode *node);
void get_W(TreeNode *root);

#endif /* __LSSS_H__ */