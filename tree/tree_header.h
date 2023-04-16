#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LOCATION __PRETTY_FUNCTION__, __FILE__, __LINE__
#define tree_ctor(tree) tree_ctor_((tree), {(#tree), LOCATION})


//! @brief type of out node values
//!
typedef double elem;


//! @brief val when operator is added in create node function
//!
const int null_val = 0xDEAD; 


//! @brief types of tree nodes
//!
enum node_type
{
    ERROR    = -1,
    
    TYPE_NUM = 0,
    OP_ADD   = 1,
    OP_SUB   = 2,
    OP_MUL   = 3,
    OP_DIV   = 4,

    TYPE_VAR = 5,
    
    FUNC_EXP = 6,
    FUNC_POW = 7,
    FUNC_SIN = 8,
    FUNC_COS = 9,
    FUNC_LOG = 10,
    FUNC_LN  = 11
};


//! @brief our tree node structure
//!
typedef struct node
{
    node *    left_child;
    node *    right_child;
    node *    parent;
    elem      value;
    node_type type;
} node_t;


//! @struct variable information
//! @brief  struct with information about variable for ctor
//! 
//! @var name - struct variable name
//! @var func - function where variable was created
//! @var file - file where variable was created
//! @var line - number of line where variable was created
//!
typedef struct
{
    const char * name;   
    const char * func;
    const char * file;
    size_t       line;
} var_info;


//! @struct location of calling function
//! @brief locaion of call
//!
//! @var func - function, where calling function is
//! @var file - file, where calling function is
//! @var line - line, where calling function is
//!
typedef struct
{
    const char * func;
    const char * file;
    size_t       line;
} location_info;


//! @brief main tree structure
//!
typedef struct
{
    var_info tree_info;
    node_t * root;

    unsigned status;
} tree_t;


//! @brief accuracy for equald
//!
const double EPSYLON = 0.000001;


//! @brief initializes tree
//!
//! @param [out] tree ptr to our tree object
//! @param [in]  info  information about our tree object creation
//!
//! @return 1 if error, 0 if ok
//!
int tree_ctor_(tree_t * tree, var_info info);


//! @brief creates new node and connects it if needed
//!
//! @param [in] type  - type of creating node
//! @param [in] value - value in node
//! @param [in] node1 - left child to connect
//! @param [in] node2 - right child to connect
//!
//! @return ptr to created node
//!
node_t * create_node(node_type type, elem value, node_t * node1 = NULL, node_t * node2 = NULL);


//! @brief destructs tree
//!
int tree_dtor(tree_t * tree);


//! @brief frees tree reccurently
//!
int tree_free(node_t * node);


//! @brief creates num node
//! 
node_t * new_num(elem value);


//! @brief creates num node
//! 
//! @param [in] type  - type of operation node
//! @param [in] node1 - left child
//! @param [in] node2 - right child 
//!
//! @return ptr to created node
//!
node_t * new_op(node_type type, node_t * node1, node_t * node2);


//! @brief deletes subtree
//!
int delete_subtree(node_t * node);


//! @brief sets tree root
//!
int tree_link_root(tree_t * tree, node_t * node);


//! @brief links 3 nodes
//!
//! @param [in] node_root - root node 
//! @param [in] node1     - left child
//! @param [in] node2     - right child 
//!
int link_nodes(node_t * node_root, node_t * node1, node_t * node2);


//! @brief creates func node
//!
node_t * new_func(node_type type, node_t * Lc, node_t * Rc);


//! @brief creates func node
//!
node_t * new_var(int name);


//! @brief checks if doubles are equal
//!
int equald(double val1, double val2);
