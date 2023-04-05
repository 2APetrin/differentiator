#pragma once
#include "../tree/tree_header.h"


//! @brief struct with our code
//!
typedef struct
{
    size_t len;         //length of reading file
    char * text_buff;   //arrange with code
} text_t;


//! @brief gets text buffer
//!
int text_ctor(text_t * text, FILE * stream);


//! @brief frees allocated memory
//!
int text_dtor(text_t * text);


//! @brief makes tree from text
//!
int make_tree(tree_t * tree, text_t * text);


//! @brief makes subtree
//!
node_t * create_subtree(text_t * text, int * index);


//! @brief calculates subtree 
//!
elem eval(node_t * node);