#include "diff_header.h"
#include "../tree/logs_header.h"
#include "../diff_DSL/DSL_header.h"
#include "../latex/latex_header.h"
#include <math.h>


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

    text->var_buff  = (var_t**) calloc (MAX_VAR_COUNT, sizeof(var_t*));
    text->var_count = 0;

    fread(text->text_buff, sizeof(char), text->len, stream);
    text->text_buff[text->len] = '\0';
    
    fclose (stream);
    return 0;
}


int text_dtor(text_t * text)
{
    ASSERT(text);
    ASSERT(text->text_buff);

    for (int i = 0; i < (int)text->len; i++) 
       text->text_buff[i] = 0;

    free(text->text_buff);
    text->text_buff = nullptr;
    
    for (int i = 0; i < text->var_count; i++)
    {
        free(text->var_buff[i]->name);
        text->var_buff[i]->name = nullptr;
        free(text->var_buff[i]);
        text->var_buff[i] = nullptr;
    }

    free(text->var_buff);
    text->var_buff = nullptr;
    text->len = 0;

    return 0;
}


node_type get_type_from_cmd(char * cmd)
{
    if (!strcmp(cmd, "exp")) return FUNC_EXP; 
    if (!strcmp(cmd, "sin")) return FUNC_SIN;
    if (!strcmp(cmd, "cos")) return FUNC_COS;
    if (!strcmp(cmd, "log")) return FUNC_LOG;
    if (!strcmp(cmd, "ln"))  return FUNC_LN;
    if (!strcmp(cmd, "^"))   return FUNC_POW;
    
    return ERROR;
}


elem eval(node_t * node)
{
    if (!node)
        return NAN;

    node_t * Lc = node->left_child;
    node_t * Rc = node->right_child;

    if (node->type == TYPE_NUM) //num
        return node->value;
    
    else             //operators and funcs
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
                {
                    elem val_r = eval(Rc);
                    elem val_l = eval(Lc);

                    if (!equald(val_r, 0))
                        return (val_l / val_r);
                    
                    else
                    {
                        fprintf(log_file, "<pre>\nERROR in eval: division by zero. Top in frac is %lg\n<pre/>", val_l);
                        return NAN;
                    }
                }
                    
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of /\n</pre>");
                return NAN;
            
            case FUNC_POW:
                if (Lc && Rc)
                {
                    elem val_r = eval(Rc);
                    elem val_l = eval(Lc);

                    if (equald(val_l, 0))
                        return (0);
                    
                    else if (val_r < 0)
                    {
                        fprintf(log_file, "<pre>\nERROR in eval: division by zero. 0^(num<0)\n<pre/>");
                        printf("division by zero: 0^(num<0)\n");
                        return NAN;
                    }

                    if (val_l < 0)
                    {
                        fprintf(log_file, "<pre>\nERROR in eval: num for pow is negative. It's illegal here\n<pre/>");
                        printf("ERROR in eval: num for pow is negative. It's illegal here\n");
                        return NAN;
                    }

                    return pow(val_l, val_r);
                }
                
                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of ^\n</pre>");
                return NAN;
            
            case FUNC_COS:
                if (Rc) return cos(eval(Rc));

                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of COS\n</pre>");
                return NAN;
            
            case FUNC_EXP:
                if (Rc) return exp(eval(Rc));

                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of EXP\n</pre>");
                return NAN;
                
            case FUNC_LN:
                if (Rc) return log(eval(Rc));

                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of LN\n</pre>");
                return NAN;
            
            case FUNC_LOG:
                if (Lc && Rc) return (log(eval(Rc)) / log(eval(Lc)));

                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of LOG\n</pre>");
                return NAN;
            
            case FUNC_SIN:
                if (Rc) return sin(eval(Rc));

                fprintf(log_file, "<pre>ERROR in eval: NULL ptr in children of SIN\n</pre>");
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
        return nullptr;
    
    switch (node->type)
    {
        case ERROR:
            return nullptr;
        
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
            if (FX_POW_GX)
            {
                node_t* ln_cpy_L = LN(copyL);
                node_t* lnfxdx = diff(ln_cpy_L);
                tree_free(ln_cpy_L);
                return MUL(copy, ADD(MUL(RC, LN(copyL)), MUL(lnfxdx, copyR)));
            }

            if (POW_CONST)
                return new_num(0);
            
            if (SHOW_FUNC)
                return MUL(copy, LN(copyR));

            if (POW_OF_X)
                return MUL(MUL(POW(copyL, SUB(copyR, new_num(1))), copyR), LC);
            
            fprintf(log_file, "<pre>\nERROR in diff function. Unknown command\n</pre>");
            return nullptr;
        }

        case FUNC_SIN: return MUL(COS(copyR), RC);
        case FUNC_COS: return MUL(new_num(-1), MUL(SIN(copyR), RC));

        case FUNC_LOG: return MUL(DIV(new_num(1), MUL(copyR, LN(copyL))), RC);

        default:
            fprintf(log_file, "<pre>\nERROR in diff function. Unknown command\n</pre>");
            return nullptr;
    }

    return nullptr;
}


node_type get_type_from_word(char * word)
{
    if (!strcmp(word, "exp")) return FUNC_EXP;
    if (!strcmp(word, "sin")) return FUNC_SIN;
    if (!strcmp(word, "cos")) return FUNC_COS;
    if (!strcmp(word, "log")) return FUNC_LOG;
    if (!strcmp(word, "ln"))  return FUNC_LN;
    
    return ERROR;
}


node_t* copy_subtree(node_t * node)
{
    if (!node)
        return nullptr;
    
    node_t * ret_node = (node_t *) calloc (1, sizeof(node_t));

    ret_node->left_child  = copy_subtree(node->left_child);
    ret_node->right_child = copy_subtree(node->right_child);

    if (ret_node->left_child) ret_node->left_child->parent = ret_node;
    if (ret_node->right_child) ret_node->right_child->parent = ret_node;
    
    ret_node->type  = node->type;
    ret_node->value = node->value;

    if (node->type == TYPE_VAR) 
    {
        ret_node->name = (char*) calloc (max_word_length, sizeof(char));
        strcpy(ret_node->name, node->name);
    }

    return ret_node;
}


int make_tree_in_order(tree_t * tree, text_t * text)
{
    ASSERT(tree);
    ASSERT(text);
    ASSERT(text->text_buff);

    tree->root = get_g(text);

    tree_dump(tree);
    
    if (!tree->root) return 1;
    return 0;
}


node_t * get_g(text_t * text)
{
    text->index = 0;
    node_t * node = get_e(text);
    
    if (text->text_buff[text->index] != '$')
    {
        fprintf(log_file, "<pre>\nERROR: index %d. Expexted $, but is: %c\n</pre>", text->index, text->text_buff[text->index]);
        tree_free(node);
        return nullptr;
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
            return nullptr;
        }

    }

    return node1;
}


node_t * get_t(text_t * text)
{
    node_t * node1 = get_d(text);

    while (text->text_buff[text->index] == '*' || text->text_buff[text->index] == '/')
    {
        int op = text->text_buff[text->index];
        text->index++;

        node_t * node2 = get_d(text);

        if      (op == '*') node1 = new_op(OP_MUL, node1, node2);
        else if (op == '/') node1 = new_op(OP_DIV, node1, node2);
        else    
        {
            fprintf(log_file, "<pre>\nERROR: index %d. Expexted * or /, but is: %c\n</pre>", text->index, op);
            return nullptr;
        }
    }    

    return node1;
}


node_t * get_d(text_t * text)
{
    node_t * node1 = get_p(text);

    if (text->text_buff[text->index] == '^')
    {
        text->index++;
        node_t * node2 = get_p(text);

        node1 = new_func(FUNC_POW, node1, node2);
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
            return nullptr;
        }

        text->index++;

        return node;
    }
    
    else if ((text->text_buff[text->index] >= '0' && text->text_buff[text->index] <= '9') || text->text_buff[text->index] == '.' || text->text_buff[text->index] == '-')
        return get_n(text);
    
    else
        return get_w(text);
    
    return nullptr;
}


node_t * get_w(text_t * text)
{
    int count = 0;
    int p     = text->index;

    char word[max_word_length] = {0};

    while (isalpha(text->text_buff[text->index]))
    {
        if (count == max_word_length - 1)
        {
            fprintf(log_file, "ERROR in getting, index - %d: word is too long - %s\n", text->index, word);
            return nullptr;
        }

        word[count] = text->text_buff[text->index];
        text->index++;
        count++;
    }

    if (p == text->index)
    {
        fprintf(log_file, "ERROR in getting word: nothing was read. And it is not a number too. Index - %d\n", text->index);
        return nullptr;
    }    

    node_type type = get_type_from_word(word);
    //printf("word - %s0\ntype - %d\n", word, type);


    if (type == ERROR)
    {
        if (strlen(word) < max_word_length)
        {
            if (!check_for_that_var(text, word)) 
            {
                text->var_buff[text->var_count] = (var_t*) calloc (1, sizeof(var_t));
                text->var_buff[text->var_count]->name = (char*) calloc (max_word_length, sizeof(char));
                
                strcpy(text->var_buff[text->var_count]->name, word);
                text->var_count++;
            }

            return new_var(word);
        }

        fprintf(log_file, "ERROR in get word. index - %d: long variable naming is not available - %s\n", text->index, word);
        return nullptr;
    }

    if (type == FUNC_LOG)
    {
        node_t * node1 = get_p(text);
        return new_func(type, node1, get_p(text));
    }

    return new_func(type, nullptr, get_p(text)); 

    fprintf(log_file, "ERROR in get word. index - %d: unknown error. word - %s\n", text->index, word);
    return nullptr;
}


int check_for_that_var(text_t * text, char * word)
{
    for (int i = 0; i < text->var_count; i++)
    {
        if (!strcmp(text->var_buff[i]->name, word)) return 1;
    }

    return 0;
}


node_t * get_n(text_t * text)
{
    int p = text->index;
    int multplr = 1;
    elem val = 0.0;

    if (text->text_buff[text->index] == '-')
    {
        multplr = -1; 
        text->index++;
        //printf("cock, %d\n", text->index);
    }

    int int_part = 1;
    int count = 10;
    while ((text->text_buff[text->index] >= '0' && text->text_buff[text->index] <= '9') || text->text_buff[text->index] == '.')
    {
        if (int_part && text->text_buff[text->index] >= '0' && text->text_buff[text->index] <= '9')
        {
            val = val * 10 + text->text_buff[text->index] - '0';
            text->index++;
        }

        if (!int_part)
        {
            val += (text->text_buff[text->index] - '0') / (double) count;
            count *= 10;
            text->index++;
        }

        if (text->text_buff[text->index] == '.' && int_part)
        {
            if (!(text->text_buff[text->index + 1] >= '0' && text->text_buff[text->index + 1] <= '9'))
            {
                fprintf(log_file, "\nERROR: index - %d, After . expected number. But get %c\n", text->index, text->text_buff[text->index + 1]);
                return nullptr;
            }
            
            text->index++;
            int_part = 0;

        }
    }
    //printf("%lg\n", val);

    if (p == text->index)
        return nullptr;

    if (equald(0.0, val))
        return new_num(0.0);

    return new_num(val * multplr);
}


int subtree_var_check(node_t * node)
{
    if (!node)
        return 0;

    if (node->type == TYPE_VAR)
        return 1;
    
    if (node->type == TYPE_NUM)
        return 0;
    
    return subtree_var_check(node->left_child) || subtree_var_check(node->right_child);
}


int tree_simplify(tree_t * tree)
{
    ASSERT(tree);

    subtree_simplify(&tree->root);
    return 0;
}


int subtree_simplify(node_t ** node)
{
    if (!(*node)) return 0;

    if ((*node)->type == TYPE_NUM || (*node)->type == TYPE_VAR)
        return 0;
    
    subtree_simplify(&((*node)->left_child));
    subtree_simplify(&((*node)->right_child));

    node_t * old_ptr = *node;
    
    if (!subtree_var_check(*node))
    {
        *node = new_num(eval(*node));
        tree_free(old_ptr);

        return 0;
    }

    if ((*node)->type == OP_MUL && (LEFT_VAL_IS(1) || RIGHT_VAL_IS(1)))
    {
        if (LEFT_VAL_IS(1))
        {
            *node = copy_subtree((*node)->right_child);
            tree_free(old_ptr);

            return 0;
        }

        else
        {
            *node = copy_subtree((*node)->left_child);
            tree_free(old_ptr);

            return 0;
        }
    }

    if ((*node)->type == OP_MUL && (LEFT_VAL_IS(0) || RIGHT_VAL_IS(0)))
    {
        *node = new_num(0);
        tree_free(old_ptr);

        return 0;
    }

    if ((*node)->type == OP_ADD && (LEFT_VAL_IS(0) || RIGHT_VAL_IS(0)))
    {
        if (LEFT_VAL_IS(0))
        {
            *node = copy_subtree((*node)->right_child);
            tree_free(old_ptr);

            return 0;
        }

        else
        {
            *node = copy_subtree((*node)->left_child);
            tree_free(old_ptr);

            return 0;
        }
    }

    if ((*node)->type == OP_SUB && (LEFT_VAL_IS(0) || RIGHT_VAL_IS(0)))
    {
        if (LEFT_VAL_IS(0))
        {
            *node =  MUL(new_num(-1), copy_subtree((*node)->right_child));
            tree_free(old_ptr);

            return 0;
        }

        else
        {
            *node = copy_subtree((*node)->left_child);
            tree_free(old_ptr);
            
            return 0;
        }
    }

    if ((*node)->type == OP_DIV && (LEFT_VAL_IS(0) || RIGHT_VAL_IS(0)))
    {
        if (RIGHT_VAL_IS(0))
        {
            printf("division by zero\n");
            *node = new_num(NAN);
            tree_free(old_ptr);

            return 0;
        }

        else
        {
            *node = new_num(0);
            tree_free(old_ptr);

            return 0;
        }
    }

    if ((*node)->type == OP_DIV && RIGHT_VAL_IS(1))
    {
        *node = copy_subtree((*node)->left_child);
        tree_free(old_ptr);

        return 0;
    }

    if ((*node)->type == FUNC_POW && (LEFT_VAL_IS(0) || RIGHT_VAL_IS(0)))
    {
        if (LEFT_VAL_IS(0) && RIGHT_VAL_IS(0))
        {
            *node = new_num(1);
            tree_free(old_ptr);

            return 0;
        }

        if (LEFT_VAL_IS(0) && (*node)->right_child->value > 0)
        {
            *node = new_num(0);
            tree_free(old_ptr);

            return 0;
        }

        if (LEFT_VAL_IS(0) && (*node)->right_child->value < 0)
        {
            printf("division by zero: 0^(num<0)\n");
            *node = new_num(NAN);
            tree_free(old_ptr);

            return 0;

            return 0;
        }

        if (RIGHT_VAL_IS(0))
        {
            *node = new_num(1);
            tree_free(old_ptr);

            return 0;
        }
    }

    if ((*node)->type == FUNC_POW && LEFT_VAL_IS(1))
    {
        *node = new_num(1);
        tree_free(old_ptr);

        return 0;
    }

    if ((*node)->type == FUNC_POW && RIGHT_VAL_IS(1))
    {
        *node = copy_subtree((*node)->left_child);
        tree_free(old_ptr);

        return 0;
    }

    return 0;
}


int substitute_var(node_t* node, text_t* text)
{
    node_t* copied_tree = copy;
    int flag = 0;

    for (int i = 0; i < text->var_count; i++)
    {
        printf("Введите число для подстановки в переменную %s в производной\n'q' - для пропуска\n", text->var_buff[i]->name);
        elem val = NAN;
        if (!scanf("%lg", &val))
        {
            if (getchar() == 'q')
            {
                while (getchar() != '\n') continue;
                continue;
            }
            printf("Неправильно, попробуй еще раз, только на этот раз подумай, угашенный блин!!!\n");
            while (getchar() != '\n') continue;
            i--;
            continue;
        }

        change_var_to_val(text->var_buff[i]->name, &copied_tree, val);
        flag = 1;
    }
    
    if (flag)
    {
        fprintf(latex_file, "\n  \\section{Подставим введенное число в производную:}\n");
        subtree_dump(copied_tree);
        latex_write_subtree(copied_tree);

        fprintf(latex_file, "\\text{Результат: }");
        subtree_simplify(&copied_tree);
        subtree_dump(copied_tree);
        latex_write_subtree(copied_tree);
    }
    
    tree_free(copied_tree);
    return 0;
}


int change_var_to_val(const char* name, node_t** node, elem val)
{
    if (!(*node)) return 0;

    CH_X_TO_VAL_L();
    CH_X_TO_VAL_R();

    if ((*node)->type == TYPE_VAR)
    {
        //printf("variable - %s, val - %lg\n", (*node)->name, val);
        if (!strcmp((*node)->name, name))
        {
            (*node)->type  = TYPE_NUM;
            (*node)->value = val;
            free((*node)->name);
        }

        return 0;
    }
    return 0;
}
