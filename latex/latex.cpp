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
    \n\\usepackage{marvosym}\n\
    \n\\setlength{\n\\topmargin}{-0.5in}\
    \n\\setlength{\n\\textheight}{9.1in}\
    \n\\setlength{\n\\oddsidemargin}{-0.4in}\
    \n\\setlength{\n\\evensidemargin}{-0.4in}\
    \n\\setlength{\n\\textwidth}{7in}\
    \n\\setlength{\n\\parindent}{0ex}\
    \n\\setlength{\n\\parskip}{1ex}\
    \n\\begin{document}\
    \n  \\title{история взятия производной}\
    \n  \\date{}\
    \n  \\maketitle\n");

    return 0;
}


void latex_create_pdf(void)
{
    close_latex_file();
    system("pdflatex latex.tex");
    system("open latex.pdf");
    system("rm latex.aux");
    system("rm latex.log");
    //system("rm latex.tex");
}