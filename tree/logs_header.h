#pragma once

#include <string.h>
#include "tree_header.h"

#define LOG_MODE

#ifdef LOG_MODE

    #define ASSERT(cond)                                                 \
    if (!(cond))                                                          \
    {                                                                      \
        printf("\nError in %s in line %d in function %s in file %s\n\n",    \
                #cond, __LINE__, __PRETTY_FUNCTION__, __FILE__),             \
        abort();                                                              \
    }

    #define tree_dump(tree) tree_dump_((tree), {LOCATION})
    #define subtree_dump(root) subtree_dump_((root), {LOCATION})


    //! @brief log_file
    //!
    extern FILE * log_file;


    //! @brief graphviz file
    //!
    extern FILE * graphviz_file;


    //! @brief extern counter for graphviz pngs
    //!
    extern int graphviz_png_count;

#endif

#ifndef LOG_MODE

    #define tree_dump(tree)

#endif


//! @brief colors for arrows
//!
enum arrow_colors
{
    RED   = 0xF80000,
    BLUE  = 0x0000FF,
    GREEN = 0x3CAA3C
};


//! @brief types of tree print
//!
enum print_mode
{
    IN_ORDER   = 0,
    PRE_ORDER  = 1,
    POST_ORDER = 2
};


//! @brief opens logfile
//!
int open_log_file(void);


//! @brief closes logfile
//!
int close_log_file(void);


//! @brief initialises graphviz file
//!
//! @return 1 if error, 0 if ok
//!
int init_graphviz_file(void);


//! @brief opens graphviz file
//!
int open_graphviz_file(void);


//! @brief closes graphviz file
//!
int close_graphviz_file(void);


//! @brief dump
//!
int tree_dump_(tree_t * tree, location_info info);


//! @brief prints tree in graphviz file
//!
int tree_print_dump(node_t * node);


//! @brief prints tree node
//!
int node_print(node_t * node);


//!@brief links tree nodes
//!
int node_link(node_t * node);


//! @brief links nodes in graphviz file
//!
int link_nodes(node_t * node1, node_t * node2, int color);


//! @brief adds node to graphviz file
//!
int graphviz_add_node(node_t * node);


//! @brief finds color from mode type
//!
//! @param [in] type - type of our node
//!
//! @return code of needed color and 0 if error
//!
int get_color(node_type type);


//! @brief deshifrates node type to string
//!
const char * get_type(node_type type);


//! @brief verifies our tree
//!
int tree_verify(tree_t * tree);


//! @brief writes dump of subtree into log file
//!
int subtree_dump_(node_t * root, location_info info);


//! @brief gets ptr to stream
//!
FILE * open_code_file(void);


//! @brief prints tree into stdout
//!
int subtree_print(node_t * node, print_mode mode, FILE * out_stream);


//! @brief prints tree in pre order
//!
int print_pre_order(node_t * node, FILE * out_stream);


//! @brief prints tree in in order
//!
int print_in_order(node_t * node, FILE * out_stream);


//! @brief prints tree in post order
//!
int print_post_order(node_t * node, FILE * out_stream);


//! @brief prints whole tree
//!
int tree_print(tree_t * tree, print_mode mode, FILE * out_stream);
