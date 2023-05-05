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

    open_latex_file();
    init_latex_file();

    

    tree_dtor(&tree);
    text_dtor(&text);
    close_log_file();
    
    latex_create_pdf();
    return 0;
}