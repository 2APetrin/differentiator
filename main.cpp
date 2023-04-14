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

    FILE * dest_file = fopen("out.txt", "w");

    //printf("value of tree - %lg\n", eval(tree.root));

    node_t * diff_tree = diff(tree.root);
    subtree_dump(diff_tree);
    subtree_print(diff_tree, IN_ORDER, dest_file);

    fclose(dest_file);
    tree_free(diff_tree);
    tree_dtor(&tree);
    text_dtor(&text);
    close_log_file();
    return 0;
}