#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    int *data;
    int length;
} rdvec;
// 定义 TokenType 枚举
typedef enum
{
    TOKEN_KEYWORD,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_EOF
} TokenType;

// 定义 Token 结构体
typedef struct
{
    TokenType type;
    char text[64];
} Token;

// 定义 TreeNode 结构体
typedef struct TreeNode
{
    char value[64];
    struct TreeNode *left;
    struct TreeNode *right;
    rdvec *vec;
} TreeNode;

// 定义 Lexer 结构体
typedef struct Lexer
{
    char *input;
    int pos;
} Lexer;

// 定义 Parser 结构体
typedef struct Parser
{
    Lexer lexer;
    Token current;
} Parser;

void init_lexer(Lexer *lexer, const char *input);
TreeNode *parse_and_or(Parser *parser);
TreeNode *get_root(char *input);
rdvec *make_rdvec();