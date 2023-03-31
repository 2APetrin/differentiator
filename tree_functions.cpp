#include "tree_header.h"
#include "logs_header.h"


int tree_ctor_(tree_t * tree, var_info info)
{
    ASSERT(tree);
    ASSERT(info.file);
    ASSERT(info.func);
    ASSERT(info.name);

    tree->tree_info = info;
    tree->root      = NULL;
    tree->status    = 0;

    tree_dump(tree);

    return 0;
}


node_t * create_node(node_type type, elem value, node_t * node1, node_t * node2)
{
    node_t * new_node = (node_t *) calloc (1, sizeof(node_t));
    
    if (!type) //число
    {
        if (node1 || node2)
        {
            printf("error: create_node you are creating nuber, but used link parameters\n");
            return NULL;
        }
        
        new_node->type  = type;
        new_node->value = value;

        return new_node;
    }

    else //команда
    {
        if (value != null_val)
        {
            printf("error: value in create_node is not null_val\n");
            return NULL;
        }

        new_node->right_child = node2;
        new_node->left_child  = node1;
        new_node->type        = type;

        return new_node;
    }

    printf("error: unknown error in create_node\n");
    return NULL;
}


int tree_dtor(tree_t * tree)
{
    ASSERT(tree);

    tree_dump(tree);

    tree_free(tree->root);
    tree->status = 0;
    tree->tree_info.file = NULL;
    tree->tree_info.func = NULL;
    tree->tree_info.name = NULL;
    tree->tree_info.line = 0;

    return 0;
}


int tree_free(node_t * node)
{
    if (node == NULL)
        return 1;
    
    if (node->left_child != NULL)
    {
        tree_free(node->left_child);
    }

    if (node->right_child != NULL)
    {
        tree_free(node->right_child);
    }

    node->value = 0;

    free(node);
    return 0;
}

