#pragma once
#include "../tree/tree_header.h"

#define PRINT_LC latex_print_in_order(node->left_child)
#define PRINT_RC latex_print_in_order(node->right_child)
#define LC_ADD_OR_SUB node->left_child->type  == OP_ADD || node->left_child->type  == OP_SUB
#define RC_ADD_OR_SUB node->right_child->type == OP_ADD || node->right_child->type == OP_SUB
#define OPEN_BRACKET  fprintf(latex_file, "(")
#define CLOSE_BRACKET fprintf(latex_file, ")")
#define LC_VAL_IS(x)  (node->left_child->type  == TYPE_NUM && equald(node->left_child->value,  (x)))
#define RC_VAL_IS(x)  (node->right_child->type == TYPE_NUM && equald(node->right_child->value, (x)))

extern FILE* latex_file;


//! @brief opens extern latex file
//!
int open_latex_file(void);


//! @brief closes extern latex file
//!
int close_latex_file(void);


//! @brief initialises latex file by writing initial lines in it
//!
int init_latex_file(void);


//! @brief makes and opens pdf
//!
void latex_create_pdf(void);


//! @brief writes latex furmula of our expression
//!
int latex_write_subtree(node_t * node); 


int latex_print_in_order(node_t * node);
