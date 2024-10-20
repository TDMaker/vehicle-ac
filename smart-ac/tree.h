#ifndef __TREE_H__
#define __TREE_H__
#include "utils.h"
#include "act_visitor.h"
#include "act_parser.h"
#include <stdbool.h>
// ptr_list get_diff(ptr_list rho1, ptr_list rho2);
TreeNode *get_sibling(TreeNode *node);
TreeNode *find_in(TreeNode *a, ptr_list b);
bool is_same_path(TreeNode *a, TreeNode *b);
void print_list(const char *name, ptr_list a);
TreeNode *find_in(TreeNode *a, ptr_list b);
TreeNode *del_from_tree(ptr_list *rho, const char *attribute);
ptr_list get_all_under_nodes(TreeNode *_node);

#endif /* __TREE_H__ */