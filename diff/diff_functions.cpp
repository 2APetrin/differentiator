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


int make_tree(tree_t * tree, text_t * text, read_mode mode)
{
    ASSERT(tree);
    ASSERT(text);
    ASSERT(text->text_buff);

    int index = 0;

    switch (mode)
    {
        case READ_PRE_ORDER:
        {
            node_t * root = create_subtree_pre_order(text, &index);
            tree->root = root;
        }
            break;

        case READ_IN_ORDER:
            //tree->root = create_subtree_in_order(text, &index);
            break;
        
        default:
            return 1;
    }

    tree_dump(tree);

    printf("index - %d\nlen - %lu", index, text->len);

    if (index != (int)text->len - 1)
        fprintf(log_file, "<pre>\nERROR in make tree: not all symbols were read\n</pre>");

    return 0;
}


node_t * create_subtree_in_order(text_t * text, int * index)
{
    ASSERT(text);

    double num = NAN;
    int length = 0;
    return 0;
}


node_t * create_subtree_pre_order(text_t * text, int * index)
{
    ASSERT(text);

    double num = NAN;
    char cmd[8] = {0};
    int length = 0;
    int length_cmd = 0;

    node_t * Lc = NULL;
    node_t * Rc = NULL;
 
    sscanf(text->text_buff + *index, "%lg%n", &num, &length);
    sscanf(text->text_buff + *index, "%[a-z^]%n", cmd, &length_cmd);
    //printf("\nval - %lg\nlen - %d\n", num, length);
    //printf("cmd - %s\nlen - %d\n", cmd, length_cmd);
    
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

    if (length_cmd)
    {
        *index += length_cmd;

        if (text->text_buff[*index] == '(' && length_cmd > 1)
        {
            --(*index);
            printf("\ncmd detected\n");
            node_type type = get_type_from_cmd(cmd);
            printf("type of cmd - %s\n", get_type(type));

            switch (type)
            {
                case FUNC_LOG:
                    Lc = NEXTsubtreePRE;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Lc && Rc)
                        return new_func(FUNC_LOG, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree: func_log (Lc && Rc) fail</pre>\n");
                    return NULL;
                
                case FUNC_POW:
                    Lc = NEXTsubtreePRE;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Lc && Rc)
                        return new_func(FUNC_POW, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree: func_pow (Lc && Rc) fail</pre>\n");
                    return NULL;
                
                case FUNC_COS:
                    Lc = NULL;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Rc)
                        return new_func(FUNC_COS, Lc, Rc);

                    return NULL;
                
                case FUNC_SIN:
                    Lc = NULL;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Rc)
                        return new_func(FUNC_SIN, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree: func_sin Rc fail</pre>\n");
                    return NULL;
                
                case FUNC_LN:
                    Lc = NULL;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Rc)
                        return new_func(FUNC_LN, Lc, Rc);

                    return NULL;
                
                case FUNC_EXP:
                    Lc = NULL;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Rc)
                        return new_func(FUNC_EXP, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree: func_exp Rc fail</pre>\n");
                    return NULL;
            }
        }

        else if (length_cmd == 1)
        {
            if (cmd[0] == '^')
            {
                Lc = NEXTsubtreePRE;
                Rc = NEXTsubtreePRE;
                ++(*index);

                printf("%p %p\n", Lc, Rc);

                if (Rc && Lc)
                    return new_func(FUNC_POW, Lc, Rc);

                return NULL;
            }

            else
            {
                printf("\nпеременная - %c\n", cmd[0]);
                printf("\n%s\n", text->text_buff + *index);
                return new_var(cmd[0]);
            }
        }
        
        else
        {
            fprintf(log_file, "<pre>ERROR in create_subtree: there is no ) after func</pre>\n");
            return NULL;
        }
    }

    switch (text->text_buff[*index])
    {
        case '(':
            return NEXTsubtreePRE;
        
        case '+':
            Lc = NEXTsubtreePRE;
            Rc = NEXTsubtreePRE;
            ++(*index);

            if (Lc && Rc)
                return new_op(OP_ADD, Lc, Rc);

            return NULL;
        
        case '-':
            Lc = NEXTsubtreePRE;
            Rc = NEXTsubtreePRE;
            ++(*index);

            if (Lc && Rc)
                return new_op(OP_SUB, Lc, Rc);
            
            return NULL;
        
        case '*':
            Lc = NEXTsubtreePRE;
            Rc = NEXTsubtreePRE;
            ++(*index);

            if (Lc && Rc)
                return new_op(OP_MUL, Lc, Rc);
            
            return NULL;
        
        case '/':
            Lc = NEXTsubtreePRE;
            Rc = NEXTsubtreePRE;
            ++(*index);

            if (Lc && Rc)
                return new_op(OP_DIV, Lc, Rc);
            
            return NULL;
        
        case ')':
            fprintf(log_file, "<pre>ERROR in create_subtree: switch catched ), index is %d\n</pre>", *index);
            break;
        
        default:
            printf("unknown error");
    }

    return NULL;
}


node_type get_type_from_cmd(char * cmd)
{
    if (!strcmp(cmd, "exp"))
        return FUNC_EXP;
    
    if (!strcmp(cmd, "sin"))
        return FUNC_SIN;
    
    if (!strcmp(cmd, "cos"))
        return FUNC_COS;

    if (!strcmp(cmd, "log"))
        return FUNC_LOG;
    
    if (!strcmp(cmd, "ln"))
        return FUNC_LN;
    
    if (!strcmp(cmd, "^"))
        return FUNC_POW;
    
    return ERROR;
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
