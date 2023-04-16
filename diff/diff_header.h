#pragma once
#include "../tree/tree_header.h"

//! @brief struct with our code
//!
typedef struct
{
    size_t len;         //length of reading file
    char * text_buff;   //arrange with code
    int    index;
} text_t;


//! @brief types of tree reading modes
//!
enum read_mode
{
    READ_IN_ORDER   = 0,
    READ_PRE_ORDER  = 1,
    READ_POST_ORDER = 2
};


//! @brief gets text buffer
//!
int text_ctor(text_t * text, FILE * stream);


//! @brief frees allocated memory
//!
int text_dtor(text_t * text);


//! @brief makes tree from text
//!
int make_tree(tree_t * tree, text_t * text, read_mode mode);


//! @brief makes subtree
//!
node_t * create_subtree_pre_order(text_t * text, int * index);


//! @brief makes subtree
//!
node_t * create_subtree_in_order(text_t * text, int * index);


//! @brief calculates subtree 
//!
elem eval(node_t * node);


//! @brief compares strings with funcs
//!
node_type get_type_from_cmd(char * cmd);


//! @brief makes new tree with differential of source function
//!
node_t * diff(node_t * node);


//! @brief copies subtree with root - node
//!
node_t * copy_subtree(node_t * node);


//! @brief new and good way to make tree
//!
int make_tree_in_order(tree_t * tree, text_t * text);


//! @brief main rule of our reader
//!
node_t * get_g(text_t * text);


//! @brief getx expanded expression / 2 rule of our reader
//!
node_t * get_e(text_t * text);


//! @brief gets * or / expression
//!
node_t * get_t(text_t * text);


//! @brief checks for ( in buffer
//!
node_t * get_p(text_t * text);


//! @brief gets number from buffer
//!
node_t * get_n(text_t * text);
