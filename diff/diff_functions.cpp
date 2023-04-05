#include "diff_header.h"
#include "../tree/logs_header.h"
#include "../diff_DSL/DSL_header.h"


int text_ctor(text_t * text, FILE * stream)
{
    ASSERT(text);
    ASSERT(stream);

    fseek(stream, 0L, SEEK_END);
    text->len = (size_t) ftell(stream);
    rewind(stream);

    text->text_buff = (char *) calloc (text->len + 1, sizeof(char));
    if (!text->text_buff)
        return 1;

    fread(text->text_buff, sizeof(char), text->len, stream);
    text->text_buff[text->len] = '\0';
    
    fclose (stream);
    return 0;
}


int text_dtor(text_t * text)
{
    ASSERT(text);
    ASSERT(text->text_buff);

    free(text->text_buff);
    text->len = 0;

    return 0;
}


int make_tree(tree_t * tree, text_t * text)
{
    ASSERT(tree);
    ASSERT(text);
    ASSERT(text->text_buff);

    int index = 0;
    tree->root = create_subtree(text, &index);
    //printf("index - %d\nlrn - %lu\n%s\n", index, text->len, text->text_buff + index);

    tree_dump(tree);

    if (index != (int)text->len - 1)
        fprintf(log_file, "<pre>\nERROR in make tree: not all symbols were read\n</pre>");

    return 0;
}


node_t * create_subtree(text_t * text, int * index)
{
    ASSERT(text);

    double num = NAN;
    int length = 0;

    sscanf(text->text_buff + *index, "%lg%n", &num, &length);
    
    if (length)
    {
        *index += length;
        if (text->text_buff[*index] == ')')
            return new_num(num);

        else
        {
            fprintf(log_file, "<pre>ERROR in create_subtree: there is no ) after num</pre>\n");
            return NULL;
        }
    }

    node_t * Lc = NULL;
    node_t * Lr = NULL;
    switch (text->text_buff[*index])
    {
        case '(':
            return NEXTsubtree;
        
        case '+':
            Lc = NEXTsubtree;
            Lr = NEXTsubtree;
            ++(*index);

            if (Lc && Lr)
                return new_op(OP_ADD, Lc, Lr);

            return NULL;
        
        case '-':
            Lc = NEXTsubtree;
            Lr = NEXTsubtree;
            ++(*index);

            if (Lc && Lr)
                return new_op(OP_SUB, Lc, Lr);
            
            return NULL;
        
        case '*':
            Lc = NEXTsubtree;
            Lr = NEXTsubtree;
            ++(*index);

            if (Lc && Lr)
                return new_op(OP_MUL, Lc, Lr);
            
            return NULL;
        
        case '/':
            Lc = NEXTsubtree;
            Lr = NEXTsubtree;
            ++(*index);

            if (Lc && Lr)
                return new_op(OP_DIV, Lc, Lr);
            
            return NULL;
        
        case ')':
            fprintf(log_file, "<pre>ERROR in create_subtree: switch catched ), index is %d\n</pre>", *index);
            break;
        
        default:
            printf("unknown error");
    }

    return NULL;
}


elem eval(node_t * node)
{
    if (!node)
        return NAN;

    node_t * Lc = node->left_child;
    node_t * Rc = node->right_child;

    if (!node->type) //num
        return node->value;
    
    else             //operator
    {
        switch (node->type)
        {
            case TYPE_NUM:
                fprintf(log_file, "<pre>ERROR in eval: switch catched num. That is strange\n</pre>");
                break;

            case OP_ADD:
                if (Lc && Rc)
                    return (eval(Lc) + eval(Rc));
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of +\n</pre>");
                return NAN;
                
            case OP_SUB:
                if (Lc && Rc)
                    return (eval(Lc) - eval(Rc));
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of -\n</pre>");
                return NAN;
            
            case OP_MUL:
                if (Lc && Rc)
                    return (eval(Lc) * eval(Rc));
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of *\n</pre>");
                return NAN;
            
            case OP_DIV:
                if (Lc && Rc)
                    return (eval(Lc) / eval(Rc));
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of /\n</pre>");
                return NAN;
            
            default:
                break;
        }

        fprintf(log_file, "<pre>ERROR in eval: unknown type of command\n</pre>");
    }

    return NAN;
}
