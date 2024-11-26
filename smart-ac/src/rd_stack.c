#include <rd_stack.h>

#define MAX_SIZE 100

Stack *rd_stk_create_stack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->array = (StkItem *)malloc(stack->capacity * sizeof(StkItem));
    return stack;
}

int rd_stk_is_empty(Stack *stack)
{
    return stack->top == -1;
}

void rd_stk_push(Stack *stack, StkItem item)
{
    if (stack->top >= (stack->capacity - 1))
    {
        printf("Stack Overflow\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        stack->array[++stack->top] = item;
        // printf("Item %s pushed to stack\n", (char *)item.ptr);
    }
}

StkItem rd_stk_pop(Stack *stack)
{
    if (rd_stk_is_empty(stack))
    {
        printf("Stack Underflow\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        StkItem item = stack->array[stack->top--];
        return item;
    }
}

StkItem rd_stk_peek(Stack *stack)
{
    if (rd_stk_is_empty(stack))
    {
        printf("Stack is empty\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        return stack->array[stack->top];
    }
}

void rd_stk_destroy_stack(Stack *stack)
{
    free(stack->array);
    free(stack);
}

// int main() {
//     Stack *stack = createStack(MAX_SIZE);

//     push(stack, 10);
//     push(stack, 20);
//     push(stack, 30);

//     printf("Top element is: %d\n", peek(stack));
//     printf("Popped element is: %d\n", pop(stack));
//     printf("Top element is now: %d\n", peek(stack));

//     destroyStack(stack);

//     return 0;
// }
