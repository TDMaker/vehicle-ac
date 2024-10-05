#ifndef __LSSS_H__
#define __LSSS_H__
#include <string.h>
#include "act_visitor.h"
#include "utils.h"

void set_childrens_vec(TreeNode *node, void *data);
void display(TreeNode *node, void *data);
TreeNode *get_complete_tree(char *input);
void rdmat_row_concat(TreeNode *node, void* data);
void get_W_rho(rdmat2 *W, char ***rho, TreeNode *root);
void rd_free_tree(TreeNode *tree);

#endif /* __LSSS_H__ */