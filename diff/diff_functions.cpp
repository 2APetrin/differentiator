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
    
    text->index = 0;

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
        
        default:
            return 1;
    }

    tree_dump(tree);

    //printf("index - %d\nlen - %lu", index, text->len);

    if (index != (int)text->len - 1)
        fprintf(log_file, "<pre>\nERROR in make tree: not all symbols were read\n</pre>");

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
            //printf("\ncmd detected\n");
            node_type type = get_type_from_cmd(cmd);
            //printf("type of cmd - %s\n", get_type(type));

            switch (type)
            {
                case FUNC_LOG:
                    Lc = NEXTsubtreePRE;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Lc && Rc)
                        return new_func(FUNC_LOG, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: func_log (Lc && Rc) fail</pre>\n");
                    return NULL;
                
                case FUNC_POW:
                    Lc = NEXTsubtreePRE;
                    Rc = NEXTsubtreePRE;
                    ++(*index);

                    if (Lc && Rc)
                        return new_func(FUNC_POW, Lc, Rc);

                    fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: func_pow (Lc && Rc) fail</pre>\n");
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

                    fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: func_sin Rc fail</pre>\n");
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

                    fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: func_exp Rc fail</pre>\n");
                    return NULL;
                
                default:
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

                //printf("%p %p\n", Lc, Rc);

                if (Rc && Lc)
                    return new_func(FUNC_POW, Lc, Rc);

                return NULL;
            }

            else
            {
                //printf("\nпеременная - %c\n", cmd[0]);
                //printf("\n%s\n", text->text_buff + *index);
                return new_var(cmd[0]);
            }
        }
        
        else
        {
            fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: there is no ) after func</pre>\n");
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
            fprintf(log_file, "<pre>ERROR in create_subtree_pre_order: switch catched ), index is %d\n</pre>", *index);
            break;
        
        default:
            fprintf(log_file, "<pre>\nunknown error in create_subtree_pre_order\n</pre>");
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


node_t * diff(node_t * node)
{
    if (!node)
        return NULL;
    
    switch (node->type)
    {
        case ERROR:
            return NULL;
        
        case TYPE_NUM: return new_num(0);
        case OP_ADD:   return ADD(LC, RC);
        case OP_SUB:   return SUB(LC, RC);
        case OP_MUL:   return ADD(MUL(LC, copyR), MUL(copyL, RC));
        case OP_DIV:   return DIV(SUB(MUL(LC, copyR), MUL(copyL, RC)), MUL(copyR, copyR));
        
        case TYPE_VAR: return new_num(1);
        
        case FUNC_EXP: return MUL(copy, RC);
        case FUNC_LN:  return MUL(DIV(new_num(1), copyR), RC);
        case FUNC_POW:
        {
            if (POW_CONST)
                return new_num(0);
            
            if (SHOW_FUNC)
                return MUL(copy, LN(copyR));

            if (POW_OF_X)
                return MUL(MUL(POW(copyL, SUB(copyR, new_num(1))), copyR), LC);
            
            if (FX_POW_GX)
                return MUL(copy, ADD(MUL(RC, LN(copyL)), MUL(DIV(new_num(1), copyL), copyL)));
        }

        case FUNC_SIN: return MUL(COS(copyR), RC);
        case FUNC_COS: return MUL(new_num(-1), MUL(SIN(copyR), RC));

        case FUNC_LOG: return MUL(DIV(new_num(1), MUL(copyR, LN(copyL))), RC);

        default:
            fprintf(log_file, "<pre>\nERROR in diff function. Unknown command\n</pre>");
            return NULL;
    }

    return NULL;
}


node_t * copy_subtree(node_t * node)
{
    if (!node)
        return NULL;
    
    node_t * ret_node = (node_t *) calloc (1, sizeof(node_t));

    ret_node->left_child  = copy_subtree(node->left_child);
    ret_node->right_child = copy_subtree(node->right_child);

    if (ret_node->left_child) ret_node->left_child->parent = ret_node;
    if (ret_node->right_child) ret_node->right_child->parent = ret_node;
    
    ret_node->type  = node->type;
    ret_node->value = node->value;  

    return ret_node;
}


int make_tree_in_order(tree_t * tree, text_t * text)
{
    ASSERT(tree);
    ASSERT(text);
    ASSERT(text->text_buff);

    tree->root = get_g(text);
    
    tree_dump(tree);
    
    return 0;
}


node_t * get_g(text_t * text)
{
    text->index = 0;
    node_t * node = get_e(text);
    
    if (text->text_buff[text->index] != '$')
    {
        fprintf(log_file, "<pre>\nERROR: index %d. Expexted $, but is: %c\n</pre>", text->index, text->text_buff[text->index]);
        return NULL;
    }

    text->index++;
    return node;
}


node_t * get_e(text_t * text)
{
    node_t * node1 = get_t(text);

    while (text->text_buff[text->index] == '+' || text->text_buff[text->index] == '-')
    {
        int op = text->text_buff[text->index];
        text->index++;

        node_t * node2 = get_t(text);

        if      (op == '+') node1 = new_op(OP_ADD, node1, node2);
        else if (op == '-') node1 = new_op(OP_SUB, node1, node2);
        else    
        {
            fprintf(log_file, "<pre>\nERROR: index %d. Expexted + or -, but is: %c\n</pre>", text->index, op);
            return NULL;
        }

    }

    return node1;
}


node_t * get_t(text_t * text)
{
    node_t * node1 = get_p(text);

    while (text->text_buff[text->index] == '*' || text->text_buff[text->index] == '/' || text->text_buff[text->index] == '^')
    {
        int op = text->text_buff[text->index];
        text->index++;

        node_t * node2 = get_p(text);

        if      (op == '*') node1 = new_op(OP_MUL, node1, node2);
        else if (op == '/') node1 = new_op(OP_DIV, node1, node2);
        else if (op == '^') node1 = new_op(FUNC_POW, node1, node2);
        else    
        {
            fprintf(log_file, "<pre>\nERROR: index %d. Expexted + or -, but is: %c\n</pre>", text->index, op);
            return NULL;
        }
    }    

    return node1;
}


node_t * get_p(text_t * text)
{
    if (text->text_buff[text->index] == '(')
    {
        text->index++;

        node_t * node = get_e(text);

        if (text->text_buff[text->index] != ')')
        {
            fprintf(log_file, "<pre>\nERROR: index %d. Expexted ) but is: %c\n</pre>", text->index, text->text_buff[text->index]);
            return NULL;
        }

        text->index++;

        return node;
    }
    
    return get_n(text);
}


node_t * get_n(text_t * text)
{
    int p = text->index;
    int val = 0;

    while (text->text_buff[text->index] >= '0' && text->text_buff[text->index] <= '9')
    {
        val = val * 10 + text->text_buff[text->index] - '0';
        text->index++;
    }

    if (p == text->index && text->text_buff[text->index + 1] >= '0' && text->text_buff[text->index + 1] <= '9')
    {
        text->index++;
        
        while (text->text_buff[text->index] >= '0' && text->text_buff[text->index] <= '9')
        {
            val = val * 10 + text->text_buff[text->index] - '0';
            text->index++;
        }
        
        val *= -1;
    }

    if (p == text->index)
    {
        fprintf(log_file, "<pre>\nERROR: index %d. Expexted number but is: %c\n</pre>", text->index, text->text_buff[text->index]);
        return NULL;
    }

    return new_num(val);
}