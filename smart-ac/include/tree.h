#ifndef __TREE_H__
#define __TREE_H__
#include "utils.h"
#include "act_visitor.h"
#include "act_parser.h"
#include <stdbool.h>
ptr_list get_all_under_nodes(TreeNode *_node);
TreeNode *get_sibling(TreeNode *node);
TreeNode *get_top(TreeNode *a);
int get_path(TreeNode *node);
TreeNode *find_node_from_tree(const char *attribute, TreeNode *node);
char *find_attribute_in(const char *a, ptr_list b);
bool is_same_path(TreeNode *a, TreeNode *b);
int is_same_tree(TreeNode *a, TreeNode *b);
void print_list(const char *name, ptr_list a);
TreeNode* add_to_tree(TreeNode *root, int path, int connector, const char *attribute);
int del_from_tree(TreeNode *node2del);
void print_tree(TreeNode *node);
#endif /* __TREE_H__ */