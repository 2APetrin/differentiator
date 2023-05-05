#include "latex_header.h"
#include "../tree/tree_header.h"
#include "../tree/logs_header.h"

FILE* latex_file = NULL;


int open_latex_file(void)
{
    if ((latex_file = fopen("latex.tex", "w")) == NULL)
    {
        printf("Cannot open latex file. Programm shutdown\n\\n");
        return 1;
    }

    return 0;
}


int close_latex_file(void)
{
    ASSERT(latex_file);

    fprintf(latex_file, "\n  \\end{center}");
    fprintf(latex_file, "\n\\end{document}");
    fclose(latex_file);

    return 0;
}


int init_latex_file(void)
{
    ASSERT(latex_file);

    fprintf(latex_file, "\\documentclass{article}\n\
    \n\\usepackage[utf8]{inputenc}\
    \n\\usepackage[T2A]{fontenc}\
    \n\\usepackage[russian]{babel}\
    \n\\usepackage{amsthm}\
    \n\\usepackage{amsmath}\
    \n\\usepackage{amssymb}\
    \n\\usepackage{tikz}\
    \n\\usepackage{textcomp}\
    \n\\usepackage{graphicx}\
    \n\\usepackage{marvosym}\
    \n\\usepackage[14pt]{extsizes}\n\
    \n\\setlength{\\topmargin}{-0.5in}\
    \n\\setlength{\\textheight}{9.1in}\
    \n\\setlength{\\oddsidemargin}{-0.4in}\
    \n\\setlength{\\evensidemargin}{-0.4in}\
    \n\\setlength{\\textwidth}{7in}\
    \n\\setlength{\\parindent}{0ex}\
    \n\\setlength{\\parskip}{1ex}\n\
    \n\\begin{document}\
    \n  \\title{история взятия одной маленькой производной}\
    \n  \\date{}\
    \n  \\maketitle\n\
    \n  \\begin{center}\n\
    \n  \\section{В начале была маленькая функция:}\n");

    return 0;
}


void latex_create_pdf(void)
{
    close_latex_file();
    system("pdflatex -interaction=batchmode latex.tex");
    system("open latex.pdf");
    system("rm latex.aux");
    system("rm latex.log");
    //system("rm latex.tex");
}


int latex_write_subtree(node_t * node)
{

    if (!node) fprintf(log_file, "WARNING: trying to write null subtree in latex_write_subtree\n");

    fprintf(latex_file, "$");
    latex_print_in_order(node);
    fprintf(latex_file, "$\n\n");

    return 0;
}


int latex_print_in_order(node_t * node)
{
    ASSERT(node);
    ASSERT(latex_file);

    switch (node->type)
    {
        case TYPE_VAR: fprintf(latex_file, "%c", (int) node->value); return 0;
        case TYPE_NUM: fprintf(latex_file, "%lg", node->value);      return 0;

        case OP_ADD:
            PRINT_LC;
            fprintf(latex_file, "+");
            PRINT_RC;
            return 0;
        
        case OP_SUB:
            PRINT_LC;
            fprintf(latex_file, "-");
            PRINT_RC;
            return 0;
        
        case OP_MUL:
            if (LC_ADD_OR_SUB || LC_VAL_IS(-1))
            {
                OPEN_BRACKET;
                PRINT_LC;
                CLOSE_BRACKET;
            }
            else
                PRINT_LC;
            
            fprintf(latex_file, "\\cdot ");

            if (RC_ADD_OR_SUB || RC_VAL_IS(-1))
            {
                OPEN_BRACKET;
                PRINT_RC;
                CLOSE_BRACKET;
            }
            else
                PRINT_RC;
            return 0;
        
        case OP_DIV:
            fprintf(latex_file, "\\frac{");
            PRINT_LC;
            fprintf(latex_file, "}{");
            PRINT_RC;
            fprintf(latex_file, "}");
            return 0;

        case FUNC_EXP:
            fprintf(latex_file, "e^{");
            PRINT_RC;
            fprintf(latex_file, "}");
            return 0;

        case FUNC_LOG:
            fprintf(latex_file, "\\log_{");
            PRINT_LC;
            fprintf(latex_file, "}(");
            PRINT_RC;
            CLOSE_BRACKET;
            return 0;
        
        case FUNC_POW:
            fprintf(latex_file, "{");
            PRINT_LC;
            fprintf(latex_file, "}^{");
            PRINT_RC;
            fprintf(latex_file, "}");
            return 0;
            

        case FUNC_SIN: fprintf(latex_file, "\\sin("); break;
        case FUNC_COS: fprintf(latex_file, "\\cos("); break;
        case FUNC_LN:  fprintf(latex_file, "\\ln(");  break;

        default:
            break;
    }

    PRINT_RC;
    CLOSE_BRACKET;

    return 0;
}