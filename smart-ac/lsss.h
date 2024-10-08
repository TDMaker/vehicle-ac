#ifndef __LSSS_H__
#define __LSSS_H__
#include <string.h>
#include "act_visitor.h"
#include "utils.h"

void set_childrens_vec(TreeNode *node, void *data);
void display(TreeNode *node, void *data);
TreeNode *get_complete_tree(char *input);
void rdmat_row_concat(TreeNode *node, void *data);
void get_W_rho(rdmat *W, TreeNode*** rho, TreeNode *root, int is_update);
void rd_free_tree(TreeNode *tree);
void init_vec(TreeNode *root);
#endif /* __LSSS_H__ */