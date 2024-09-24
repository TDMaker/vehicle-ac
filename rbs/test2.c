#include <stdio.h>
typedef struct 
{
    char* name;
} rdtext;

void modify(const rdtext tmp)
{
    tmp.name[5] = 'X';
}
int main(void)
{
    char myname[] = "randy1234";
    rdtext tmp = {. name = myname};
    modify(tmp);
    puts(tmp.name);
    return 0;
}