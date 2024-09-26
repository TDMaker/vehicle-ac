#include "act_parser.h"
// 定义 QueueNode 结构体
typedef struct QueueNode
{
    TreeNode *node;
    struct QueueNode *next;
} QueueNode;

// 定义 Queue 结构体
typedef struct
{
    QueueNode *front;
    QueueNode *rear;
} Queue;

typedef void (*my_operator)(TreeNode *a);

typedef enum {
    PADDING,
    FILLING,
    DISPLAY,
} OP_TYPE;

void breadth_first_traversal(TreeNode *root, my_operator op);