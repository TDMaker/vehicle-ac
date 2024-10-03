#include "act_visitor.h"
extern int counter;

// 初始化空队列
void init_queue(Queue *queue)
{
    queue->front = NULL;
    queue->rear = NULL;
}

// 检查队列是否为空
int is_empty(Queue *queue)
{
    return queue->front == NULL;
}

// 入队
void enqueue(Queue *queue, TreeNode *node)
{
    QueueNode *new_node = (QueueNode *)malloc(sizeof(QueueNode));
    new_node->node = node;
    new_node->next = NULL;

    if (is_empty(queue))
    {
        queue->front = new_node;
    }
    else
    {
        queue->rear->next = new_node;
    }
    queue->rear = new_node;
}

// 出队
TreeNode *dequeue(Queue *queue)
{
    if (is_empty(queue))
    {
        return NULL;
    }

    QueueNode *temp = queue->front;
    TreeNode *node = temp->node;
    queue->front = queue->front->next;

    if (queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    return node;
}

// 广度优先遍历二叉树
void breadth_first_traversal(TreeNode *root, my_operator op, void* data)
{
    if (root == NULL)
    {
        return;
    }

    Queue queue;
    init_queue(&queue);
    enqueue(&queue, root);

    while (!is_empty(&queue))
    {
        TreeNode *current = dequeue(&queue);
        op(current, data);

        if (current->left)
        {
            enqueue(&queue, current->left);
        }
        if (current->right)
        {
            enqueue(&queue, current->right);
        }
    }
}