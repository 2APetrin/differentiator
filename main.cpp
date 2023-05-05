#include "tree/tree_header.h"
#include "tree/logs_header.h"
#include "diff/diff_header.h"
#include "latex/latex_header.h"

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
    make_tree_in_order(&tree, &text);
    tree_dump(&tree);

    open_latex_file();
    init_latex_file();

    latex_write_subtree(tree.root);
    subtree_simplify(&tree.root);
    tree_dump(&tree);
    latex_write_subtree(tree.root);

    fprintf(latex_file, "\n  \\section{Потом у нее появился друг, ее производная:}\n");

    node_t * diff_tree = diff(tree.root);
    subtree_dump(diff_tree);
    latex_write_subtree(diff_tree);

    subtree_simplify(&diff_tree);
    subtree_dump(diff_tree);
    latex_write_subtree(diff_tree);

    tree_free(diff_tree);
    tree_dtor(&tree);
    text_dtor(&text);
    close_log_file();
    latex_create_pdf();
    return 0;
}