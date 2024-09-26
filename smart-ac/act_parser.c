#include "act_parser.h"
#define CAPACITY 1024
int counter = 1;
// 初始化 Lexer
void init_lexer(Lexer *lexer, const char *input)
{
    lexer->input = strdup(input);
    lexer->pos = 0;
}

// 获取下一个标记
Token next_token(Parser *parser)
{
    Lexer *lexer = &parser->lexer;
    parser->current.type = TOKEN_EOF; // 假定为 EOF，以便初始化

    if (lexer->input[lexer->pos] == '\0')
    {
        return parser->current;
    }

    int start_pos = lexer->pos;

    if (isspace(lexer->input[lexer->pos]))
    {
        lexer->pos++;
    }
    else if (lexer->input[lexer->pos] == '(')
    {
        lexer->pos++;
        parser->current.type = TOKEN_LPAREN;
        parser->current.text[0] = '(';
        parser->current.text[1] = '\0';
    }
    else if (lexer->input[lexer->pos] == ')')
    {
        lexer->pos++;
        parser->current.type = TOKEN_RPAREN;
        parser->current.text[0] = ')';
        parser->current.text[1] = '\0';
    }
    else if (lexer->input[lexer->pos] == '&')
    {
        lexer->pos += 2; // 跳过 "&&"
        parser->current.type = TOKEN_AND;
        parser->current.text[0] = '&';
        parser->current.text[1] = '&';
        parser->current.text[2] = '\0';
    }
    else if (lexer->input[lexer->pos] == '|')
    {
        lexer->pos += 2; // 跳过 "||"
        parser->current.type = TOKEN_OR;
        parser->current.text[0] = '|';
        parser->current.text[1] = '|';
        parser->current.text[2] = '\0';
    }
    else
    {
        while (!isspace(lexer->input[lexer->pos]) && lexer->input[lexer->pos] != '\0' && lexer->input[lexer->pos] != '(' && lexer->input[lexer->pos] != ')')
        {
            lexer->pos++;
        }
        int len = lexer->pos - start_pos;
        strncpy(parser->current.text, lexer->input + start_pos, len);
        parser->current.text[len] = '\0';
        parser->current.type = TOKEN_KEYWORD;
    }

    return parser->current;
}

// 解析布尔表达式

TreeNode *parse_expression(Parser *parser)
{
    Token token = next_token(parser);
    if (token.type == TOKEN_LPAREN)
    {
        TreeNode *node = parse_and_or(parser);
        Token close_paren = next_token(parser);
        if (close_paren.type != TOKEN_RPAREN)
        {
            fprintf(stderr, "Expected ')'\n");
            exit(1);
        }
        return node;
    }
    else if (token.type == TOKEN_KEYWORD)
    {
        TreeNode *node = (TreeNode *)malloc(sizeof(TreeNode));
        strcpy(node->value, token.text);
        node->left = NULL;
        node->right = NULL;
        return node;
    }
    else
    {
        fprintf(stderr, "Unexpected token '%s'\n", token.text);
        exit(1);
    }
}

TreeNode *parse_and_or(Parser *parser)
{
    TreeNode *node = parse_expression(parser);
    while (true)
    {
        Token token = next_token(parser);
        if (token.type == TOKEN_AND)
        {
            TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
            strcpy(new_node->value, "&&");
            new_node->left = node;
            new_node->right = parse_expression(parser);
            node = new_node;
        }
        else if (token.type == TOKEN_OR)
        {
            TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
            strcpy(new_node->value, "||");
            new_node->left = node;
            new_node->right = parse_expression(parser);
            node = new_node;
        }
        else
        {
            parser->lexer.pos--; // 回退到当前位置
            return node;
        }
    }
}

// 释放树的内存
void free_tree(TreeNode *node)
{
    if (node == NULL)
        return;
    free_tree(node->left);
    free_tree(node->right);
    free(node);
}

rdvec *make_rdvec()
{
    rdvec *tmp = (rdvec *)malloc(sizeof(rdvec));
    if (tmp == NULL)
    {
        puts("mcalloc failed, exiting...");
        exit(-1);
    }
    (*tmp).length = 1;
    tmp->data = (int *)calloc(sizeof(int), CAPACITY);
    if (tmp->data == NULL)
    {
        puts("mcalloc failed, exiting...");
        exit(-1);
    }
    // printf("The addr of the new alloced vec is %p.\n", tmp);
    return tmp;
}

TreeNode *get_root(char *input)
{
    Lexer lexer;
    Parser parser = {.lexer = lexer};
    init_lexer(&parser.lexer, input);

    return parse_and_or(&parser);
}