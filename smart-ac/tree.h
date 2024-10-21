#ifndef __TREE_H__
#define __TREE_H__
#include "utils.h"
#include "act_visitor.h"
#include "act_parser.h"
#include <stdbool.h>
// ptr_list get_diff(ptr_list rho1, ptr_list rho2);
TreeNode *get_sibling(TreeNode *node);
TreeNode *find_node_in(TreeNode *a, ptr_list b);
TreeNode *find_attribute_in(const char *a, ptr_list b);
bool is_same_path(TreeNode *a, TreeNode *b);
int is_same_tree(TreeNode *a, TreeNode *b);
void print_list(const char *name, ptr_list a);
TreeNode *find_in(TreeNode *a, ptr_list b);
TreeNode *del_from_tree(ptr_list *rho, const char *attribute);
ptr_list get_all_under_nodes(TreeNode *_node);
TreeNode *add_to_tree(ptr_list rho, TreeNode *orign_node, TreeNode *sibling_in_new_rho);
TreeNode *get_node_in_another_tree(TreeNode *leaf_node1, ptr_list new_rho);
void branch_it(TreeNode *target_node, TreeNode *template_node, element_t *lambda);
TreeNode *get_top(TreeNode *a);
#endif /* __TREE_H__ */