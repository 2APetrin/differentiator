#include "tree/tree_header.h"
#include "tree/logs_header.h"
#include "diff/diff_header.h"

//спросить деда про баг с переменой мест в сreate subtree

int main(void)
{
    open_log_file();
    FILE * code = open_code_file();
    if (!code)
    {
        close_log_file();
        return 1;
    }

    text_t text = {};
    text_ctor(&text, code);

    tree_t tree = {};
    tree_ctor(&tree);
    make_tree(&tree, &text, READ_PRE_ORDER);

    printf("\n\n");
    for (int i = 0; i < text.len; i++)
        printf("%d - %c\n", i, text.text_buff[i]);

    FILE * dest_file = fopen("out.txt", "w");
    if (dest_file)
        tree_print(&tree, PRE_ORDER, dest_file);

    //printf("value of tree - %lg\n", eval(tree.root));

    tree_dtor(&tree);
    text_dtor(&text);
    close_log_file();
    return 0;
}