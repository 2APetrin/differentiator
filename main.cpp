#include "tree_header.h"
#include "logs_header.h"

int main(void)
{
    open_log_file();

    tree_t tree = {};
    tree_ctor(&tree);

    node_t * n1 = create_node(NUM, 12);
    node_t * n2 = create_node(NUM, 13);
    node_t * n3 = create_node(OP_ADD, null_val, n1, n2);
    tree.root = n3;

    tree_dump(&tree);

    node_t * n4 = create_node(NUM, -14);
    node_t * n5 = create_node(OP_MUL, null_val, n3, n4);
    tree.root = n5;
    
    tree_dump(&tree);

    node_t * n6 = create_node(NUM, 28);
    node_t * n7 = create_node(OP_SUB, null_val, n6, n5);
    tree.root = n7;

    tree_dump(&tree);

    node_t * n8 = create_node(NUM, 01.43);
    node_t * n9 = create_node(OP_SUB, null_val, n7, n8);

    tree.root = n9;

    tree_dump(&tree);

    close_log_file();
    return 0;
}