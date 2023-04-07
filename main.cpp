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
    //make_tree(&tree, &text);

    node_t * n1 = new_var('x');
    node_t * n2 = new_num(100);
    node_t * n3 = new_func(FUNC_EXP, NULL, n1);
    node_t * n4 = new_op(OP_ADD, n2, n3);
    tree_link_root(&tree, n4);
    tree_dump(&tree);

    FILE * dest_file = fopen("out.txt", "w");
    //if (dest_file)
    //    tree_print(&tree, IN_ORDER, dest_file);

    //printf("value of tree - %lg\n", eval(tree.root));

    tree_dtor(&tree);
    text_dtor(&text);
    close_log_file();
    return 0;
}