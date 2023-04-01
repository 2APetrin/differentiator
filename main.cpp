#include "tree/tree_header.h"
#include "tree/logs_header.h"

int main(void)
{
    open_log_file();

    tree_t tree = {};
    tree_ctor(&tree);

    node_t * n1 = new_num(12);
    node_t * n2 = new_num(13);
    node_t * n3 = new_op(OP_ADD, n1, n2);
    tree.root = n3;

    tree_dump(&tree);

    node_t * n4 = new_num(14);
    node_t * n5 = new_op(OP_MUL, n3, n4);
    tree.root = n5;
    
    tree_dump(&tree);

    node_t * n6 = new_num(28);
    node_t * n7 = create_node(OP_SUB, null_val, n6, n5);
    tree.root = n7;

    tree_dump(&tree);

    node_t * n8 = create_node(TYPE_NUM, 01.43);
    node_t * n9 = create_node(OP_DIV, null_val, n7, n8);

    tree.root = n9;

    tree_dump(&tree);

    delete_subtree_or_node(n3);

    tree_dump(&tree);

    delete_subtree_or_node(n4);

    subtree_dump(n7);

    tree_dtor(&tree);
    close_log_file();
    return 0;
}