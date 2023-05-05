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
            fprintf(log_file, "<pre>\nerror: create_node you are creating nuber, but used link parameters\n</pre>");
            return NULL;
        }

        new_node->left_child  = NULL;
        new_node->right_child = NULL;
        
        new_node->type  = type;
        new_node->value = value;

        return new_node;
    }

    else if (type < TYPE_VAR && type != ERROR) //команда
    {
        if (!equald(value, null_val))
        {
            printf("error: value in create_node is not null_val\n");
            return NULL;
        }

        if (!(node1 && node2))
        {
            fprintf(log_file, "<pre>\nerror: create_node - one of children is null (operator)\n</pre>");
            return NULL;
        }

        new_node->right_child = node2;
        new_node->left_child  = node1;

        new_node->value = value;
        new_node->type  = type;
        node1->parent   = new_node;
        node2->parent   = new_node;

        subtree_dump(new_node);

        return new_node;
    }

    else if (type == TYPE_VAR)
    {
        if (node1 || node2)
        {
            fprintf(log_file, "<pre>\nerror: create_node you are creating variable, but used link parameters\n</pre>");
            return NULL;
        }

        new_node->left_child  = NULL;
        new_node->right_child = NULL;
        
        new_node->value = value;
        new_node->type  = type;

        return new_node;
    }

    else //функции
    {
        if (type == FUNC_POW || type == FUNC_LOG)
        {
            if (!(node1 && node2))
            {
                fprintf(log_file, "<pre>\nerror: create_node - one of children is null (functions pow and log)\n</pre>");
                return NULL;
            }

            new_node->left_child  = node1;
            new_node->right_child = node2;

            new_node->type  = type;
            new_node->value = null_val;
            node1->parent   = new_node;
            node2->parent   = new_node;

            subtree_dump(new_node);

            return new_node;
        }

        else
        {
            if (node1)
            {
                fprintf(log_file, "<pre>\nerror: create_node you are creating func with one argument, but used link left parameter\n</pre>");
                return NULL;
            }

            new_node->left_child  = NULL;
            new_node->right_child = node2;

            new_node->type  = type;
            new_node->value = null_val;
            node2->parent   = new_node;

            subtree_dump(new_node);

            return new_node;
        }
    }

    printf("error: unknown error in create_node\n");
    return NULL;
}


int tree_dtor(tree_t * tree)
{
    ASSERT(tree);

    tree_verify(tree);

    //tree_dump(tree);

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

    free(node);

    return 0;
}


node_t * new_num(elem value)
{
    return create_node(TYPE_NUM, value);
}


node_t * new_op(node_type type, node_t * node1, node_t * node2)
{
    return create_node(type, null_val, node1, node2);
}


int delete_subtree(node_t * node)
{
    ASSERT(node);

    if (node->parent->left_child == node)
    {
        node->parent->left_child = NULL;
        tree_free(node);

        return 0;
    }

    else if (node->parent->right_child == node)
    {
        node->parent->right_child = NULL;
        tree_free(node);

        return 0;
    }
    
    return 1;
}


int tree_link_root(tree_t * tree, node_t * node)
{
    ASSERT(tree);
    ASSERT(node);

    tree->root = node;
    return 0;
}


int link_nodes(node_t * node_root, node_t * node1, node_t * node2)
{
    ASSERT(node_root);
    ASSERT(node1);
    ASSERT(node2);

    node_root->left_child  = node1;
    node_root->right_child = node2;
    return 0;
}


node_t * new_func(node_type type, node_t * Lc, node_t * Rc)
{
    return create_node(type, null_val, Lc, Rc);
}


node_t * new_var(int name)
{
    return create_node(TYPE_VAR, name);
}


int equald(double val1, double val2)
{
    if (!isfinite(val1)) return 0;
    if (!isfinite(val2)) return 0;
    
    if (val2 - val1 < EPSYLON && val2 - val1 > -1 * EPSYLON)
        return 1;
    
    return 0;
}


int _tree_verify(tree_t * tree, location_info loc_inf)
{
    ASSERT(tree);

    unsigned error_count = 0;

    int error_number = tree_err_check(tree, &error_count);
    if (!error_number) return 0;

    unsigned switch_param = 0;
    int flag = 1;

    for (int i = 0; i < (int) error_count; i++)
    {
        switch_param = error_number & (1 << i);
        switch (switch_param)
        {
            case NULL_ROOT:
                fprintf(log_file, "WARNING in tree check: tree root is null in: file - %s\nfunc - %s\nline - %lu\n", loc_inf.file, loc_inf.func, loc_inf.line);
                flag = 0;
                break;
            
            case BAD_STATUS:
                fprintf(log_file, "ERROR: tree status isn't 0 in: file - %s\nfunc - %s\nline - %lu\n", loc_inf.file, loc_inf.func, loc_inf.line);
                flag = 1;
                break;
            
            default:
                break;
        }
    }

    if (flag) return 1;

    return 0;
}


int tree_err_check(tree_t * tree, unsigned * err_count)
{
    ASSERT(tree);
    ASSERT(err_count);

    int ret_num = 0;

    if (tree->root == NULL)
    {
        ret_num += NULL_ROOT;
        *err_count += 1;
    }

    if (tree->status != 0)
    {
        ret_num += BAD_STATUS;
        *err_count += 1;
    }

    return ret_num;
}