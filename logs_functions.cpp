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

    fprintf(log_file, "\n<pre>\n    dump\ndump from:\nfile - %s\nfunc - %s\nline - %lu\n\ntree - %p\nroot - %p\n</pre>\n", info.file, info.func, info.line, tree, tree->root);

    tree_print(tree->root);

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

    fprintf(graphviz_file, "    node_%p[shape = Mrecord, label = \"{{%p} | {type = %d} | {value = %lg} | {%p | %p}}\", style = \"filled\", fillcolor = \"#%X\"];\n", node, node, node->type, node->value, node->left_child, node->right_child, (unsigned) get_color(node->type));
    
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


int tree_print(node_t * root)
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

    char sys_cmd[200] = "dot logs/log_graphviz.dot -Tpng -o logs/list_dump";
    snprintf(sys_cmd + strlen(sys_cmd), 30, "%d.png", graphviz_png_count);
    system(sys_cmd);

    fprintf(log_file, "\n<img src=\"list_dump%d.png\" width=\"30%%\">\n", graphviz_png_count);

    graphviz_png_count++;

    return 0;
}


int get_color(node_type type)
{
    switch (type)
    {
    case NUM:
        {
            return 0xFAEEDD;
        }
        break;
    
    case OP_ADD:
        {
            return 0x8CCB5E;
        }
        break;
    
    case OP_SUB:
        {
            return 0x8CCB5E;
        }
        break;
    
    case OP_MUL:
        {
            return 0xE28090;
        }
        break;
    
    case OP_DIV:
        {
            return 0xE28090;
        }
        break;

    default:
        break;
    }

    return 0;
}