#ifndef __TREE_H__
#define __TREE_H__
#include "utils.h"
#include "act_parser.h"
#include <stdbool.h>
ptr_list get_diff(ptr_list rho1, ptr_list rho2);

typedef struct {
    ptr_list deleted_attributes_connected_by_or;
    ptr_list deleted_attributes_connected_by_and;
    ptr_list added_attributes_connected_by_or;
    ptr_list added_attributes_connected_by_and;
    ptr_list replaced_attributes_connected_by_or;
    ptr_list replaced_attributes_connected_by_and;
} Result;
Result get_result(ptr_list rho1, ptr_list rho2);
#endif /* __TREE_H__ */