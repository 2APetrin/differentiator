#include "logs_header.h"
#include "tree_header.h"


FILE * log_file;
FILE * graphviz_file;
int graphviz_png_count = 0;


int open_log_file(void)
{
    if ((log_file = fopen("logs/log_file.html", "w")) == NULL)
    {
        printf("Cannot open log_file. Programm shutdown\n");
        return 1;
    }

    fprintf(log_file, "<html>\n");
    return 0;
}


int close_log_file(void)
{
    ASSERT(log_file);

    fprintf(log_file, "\n</html>");
    
    fclose(log_file);
    return 0;
}


int open_graphviz_file(void)
{
    if ((graphviz_file = fopen("logs/log_graphviz.dot", "w")) == NULL)
    {
        printf("Cannot open graphviz file. Programm shutdown\n");
        return 1;
    }

    return 0;
}


int close_graphviz_file(void)
{
    ASSERT(graphviz_file);

    fprintf(graphviz_file, "}");
    
    fclose(graphviz_file);
    return 0;
}


int tree_dump_(tree_t * tree, location_info info)
{
    ASSERT(tree);
    ASSERT(info.file);
    ASSERT(info.func);
    //printf("dump %s %s %lu\n", info.file, info.func, info.line);

    fprintf(log_file, "\n<pre>\n\n\n    dump\ndump from:\nfile - %s\nfunc - %s\nline - %lu\n\ntree - %p\nroot - %p\n</pre>\n", info.file, info.func, info.line, tree, tree->root);

    tree_print_dump(tree->root);

    return 0;
}


int init_graphviz_file(void)
{
    ASSERT(graphviz_file);

    fprintf(graphviz_file, "digraph\n{\n");

    return 0;
}


int node_print(node_t * node)
{
    if (node == NULL)
        return 1;

    if (node->left_child != NULL)
        node_print(node->left_child);

    if (node->right_child != NULL)
        node_print(node->right_child);
    
    graphviz_add_node(node);

    return 0;
}


int graphviz_add_node(node_t * node)
{
    ASSERT(node);
    ASSERT(graphviz_file);

    if (node->type == TYPE_VAR)
        fprintf(graphviz_file, "    node_%p[shape = Mrecord, label = \"{{%p} | {parent - %p} | {%s} | {%s} | {%p|%p}}\", style = \"filled\", fillcolor = \"#%X\"];\n", node, node, node->parent, get_type(node->type), node->name, node->left_child, node->right_child, (unsigned) get_color(node->type));    
    
    else if (node->type > TYPE_VAR)
        fprintf(graphviz_file, "    node_%p[shape = Mrecord, label = \"{{%p} | {parent - %p} | {%s} | {%p|%p}}\", style = \"filled\", fillcolor = \"#%X\"];\n", node, node, node->parent, get_type(node->type), node->left_child, node->right_child, (unsigned) get_color(node->type));    
    
    else
        fprintf(graphviz_file, "    node_%p[shape = Mrecord, label = \"{{%p} | {parent - %p} | {%s} | {value = %lg} | {%p|%p}}\", style = \"filled\", fillcolor = \"#%X\"];\n", node, node, node->parent, get_type(node->type), node->value, node->left_child, node->right_child, (unsigned) get_color(node->type));
    
    return 0;
}


int link_nodes(node_t * node1, node_t * node2, int color)
{
    ASSERT(node1);
    ASSERT(node2);
    ASSERT(graphviz_file);

    fprintf(graphviz_file, "    node_%p->node_%p [color = \"#%X\"];\n", node1, node2, (unsigned) color);
    return 0;
}


int node_link(node_t * node)
{
    if (node == NULL)
        return 1;

    if (node->left_child != NULL)
    {
        link_nodes(node, node->left_child, RED);
        node_link(node->left_child);
    }

    if (node->right_child != NULL)
    {
        link_nodes(node, node->right_child, RED);
        node_link(node->right_child);
    }

    return 0;
}


int tree_print_dump(node_t * root)
{
    if (root == NULL)
    {
        return 1;
    }

    open_graphviz_file();
    init_graphviz_file();

    node_print(root);
    node_link(root);

    close_graphviz_file();

    char sys_cmd[200] = "dot logs/log_graphviz.dot -Tpng -o logs/images/list_dump";
    snprintf(sys_cmd + strlen(sys_cmd), 30, "%d.png", graphviz_png_count);
    system(sys_cmd);

    fprintf(log_file, "\n<img src=\"images/list_dump%d.png\" width=\"60%%\">\n", graphviz_png_count);

    graphviz_png_count++;

    return 0;
}


int get_color(node_type type)
{
    switch (type)
    {
    case TYPE_NUM:
        return 0xFAEEDD;
    
    case OP_ADD:
        return 0x8CCB5E;
    
    case OP_SUB:
        return 0x8CCB5E;
    
    case OP_MUL:
        return 0xACABF2;
    
    case OP_DIV:
        return 0xACABF2;
    
    case TYPE_VAR:
        return 0xFF9ED5;

    case FUNC_EXP:
        return 0xFDDB6D;
    
    case FUNC_COS:
        return 0xFDDB6D;
    
    case FUNC_SIN:
        return 0xFDDB6D;

    case FUNC_LN:
        return 0xFDDB6D;

    case FUNC_POW:
        return 0xFDDB6D;
    
    case FUNC_LOG:
        return 0xFDDB6D;
    
    case ERROR:
        return 0xE28090;

    default:
        break;
    }

    return 0;
}


const char * get_type(node_type type)
{
    switch (type)
    {
        case TYPE_NUM:
            return "num";
        
        case OP_ADD:
            return "+";

        case OP_SUB:
            return "-";

        case OP_MUL:
            return "*";
        
        case OP_DIV:
            return "/";
        
        case TYPE_VAR:
            return "var";
        
        case FUNC_EXP:
            return "exp";
        
        case FUNC_COS:
            return "cos";
        
        case FUNC_SIN:
            return "sin";

        case FUNC_LN:
            return "ln";
        
        case FUNC_LOG:
            return "log";

        case FUNC_POW:
            return "^";

        case ERROR:
            return "error";

        default:
            return NULL;
    }

    return NULL;
}


int subtree_dump_(node_t * root, location_info info)
{
    ASSERT(info.file);
    ASSERT(info.func);

    if (root == NULL)
        return 0;

    fprintf(log_file, "\n<pre>\n\n\n    subtree dump\ndump from:\nfile - %s\nfunc - %s\nline - %lu\n\nroot - %p\n</pre>\n", info.file, info.func, info.line, root);

    tree_print_dump(root);

    return 0;
}


int subtree_print(node_t * node, print_mode mode, FILE * out_stream)
{
    if (node == NULL)
        return 1;

    switch (mode)
    {
        case IN_ORDER:
            print_in_order(node, out_stream);
            break;

        case PRE_ORDER:
            print_pre_order(node, out_stream);
            break;

        case POST_ORDER:
            print_post_order(node, out_stream);
            break;

        default:
            fprintf(log_file, "<pre>\nERROR in subtree print\nbad print mode\n</pre>");
            return 1;
    }

    return 0;
}


int print_in_order(node_t * node, FILE * out_stream)
{
    fprintf(out_stream, "(");

    if (node->left_child != NULL)
    {
        if (node->type == FUNC_LOG)
        {
            fprintf(out_stream, "%s", get_type(node->type));
            print_in_order(node->left_child, out_stream);
            print_in_order(node->right_child, out_stream);
            fprintf(out_stream, ")");
            return 0;
        }

        print_in_order(node->left_child, out_stream);
    }

    if (!node->type)
        fprintf(out_stream, "%lg", node->value);
    
    else if (node->type != TYPE_VAR)
    {
        fprintf(out_stream, "%s", get_type(node->type));
    }

    else
    {
        fprintf(out_stream, "%c", (int)node->value);
    }

    if (node->right_child != NULL)
    {
        print_in_order(node->right_child, out_stream);
    }

    fprintf(out_stream, ")");

    return 0;
}


int print_pre_order(node_t * node, FILE * out_stream)
{
    fprintf(out_stream, "(");

    if (!node->type)
        fprintf(out_stream, "%lg", node->value);
    
    else if (node->type != TYPE_VAR)
    {
        fprintf(out_stream, "%s", get_type(node->type));
    }

    else
    {
        fprintf(out_stream, "%c", (int)node->value);
    }

    if (node->left_child != NULL)
    {
        print_pre_order(node->left_child, out_stream);
    }

    if (node->right_child != NULL)
    {
        print_pre_order(node->right_child, out_stream);
    }

    fprintf(out_stream, ")");

    return 0;
}

int print_post_order(node_t * node, FILE * out_stream)
{
    fprintf(out_stream, "(");

    if (node->left_child != NULL)
    {
        print_post_order(node->left_child, out_stream);
    }

    if (node->right_child != NULL)
    {
        print_post_order(node->right_child, out_stream);
    }

    if (!node->type)
        fprintf(out_stream, "%lg", node->value);
    
    else if (node->type != TYPE_VAR)
    {
        fprintf(out_stream, "%s", get_type(node->type));
    }

    else
    {
        fprintf(out_stream, "%c", (int)node->value);
    }

    fprintf(out_stream, ")");

    return 0;
}


int tree_print(tree_t * tree, print_mode mode, FILE * out_stream)
{
    ASSERT(tree);

    if (tree->root == NULL)
        return 1;

    subtree_print(tree->root, mode, out_stream);

    return 0;
}


FILE * open_code_file(void)
{
    FILE * fp = NULL;
    
    if (!(fp = fopen("code.txt", "r")))
    {
        printf("Cannot open code_file\n");
        return NULL;
    }

    return fp;
}
