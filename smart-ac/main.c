#include <stdlib.h>
#include <string.h>
#include "lsss.h"

// 主函数
int main()
{
    char input[] = "(E)&&((((A)&&(B))||((C)&&(D)))||(((A)||(B))&&((C)||(D))))";

    get_complete_tree(input);
    
    return 0;
}