#define NEXTsubtreePRE create_subtree_pre_order(text, &(++(*index)))

#define LC diff(node->left_child)
#define RC diff(node->right_child)

#define copyL copy_subtree(node->left_child)
#define copyR copy_subtree(node->right_child)
#define copy  copy_subtree(node)

#define Lc_diff 

#define ADD(Lc, Rc) new_op(OP_ADD, Lc, Rc)
#define SUB(Lc, Rc) new_op(OP_SUB, Lc, Rc)
#define MUL(Lc, Rc) new_op(OP_MUL, Lc, Rc)
#define DIV(Lc, Rc) new_op(OP_DIV, Lc, Rc)

#define POW(Lc, Rc) new_func(FUNC_POW, Lc, Rc)
#define LN(Ch)  new_func(FUNC_LN, NULL, Ch)
#define SIN(Ch) new_func(FUNC_SIN, NULL, Ch)
#define COS(Ch) new_func(FUNC_COS, NULL, Ch)

#define POW_CONST node->left_child->type == TYPE_NUM && node->right_child->type == TYPE_NUM
#define SHOW_FUNC node->left_child->type == TYPE_NUM && node->right_child->type != TYPE_NUM
#define POW_OF_X  node->left_child->type != TYPE_NUM && node->right_child->type == TYPE_NUM
#define FX_POW_GX node->left_child->type != TYPE_NUM && node->right_child->type != TYPE_NUM

#define LOG_CONST node->left_child->type == TYPE_NUM && node->right_child->type == TYPE_NUM
#define LOG_ONLY_ARG_X 
