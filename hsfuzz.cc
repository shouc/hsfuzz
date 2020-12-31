/*
  +----------------------------------------------------------------------+
  | Skeleton PHP extension                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2018 NAME                                              |
  +----------------------------------------------------------------------+
  | Permission is hereby granted, free of charge, to any person          |
  | obtaining a copy of this software and associated documentation files |
  | (the "Software"), to deal in the Software without restriction,       |
  | including without limitation the rights to use, copy, modify, merge, |
  | publish, distribute, sublicense, and/or sell copies of the Software, |
  | and to permit persons to whom the Software is furnished to do so,    |
  | subject to the following conditions:                                 |
  |                                                                      |
  | The above copyright notice and this permission notice shall be       |
  | included in all copies or substantial portions of the Software.      |
  |                                                                      |
  | THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      |
  | EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF   |
  | MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                |
  | NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS  |
  | BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN   |
  | ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN    |
  | CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE     |
  | SOFTWARE.                                                            |
  +----------------------------------------------------------------------+
  | Author: NAME <EMAIL@EXAMPLE.COM>                                     |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_pcon.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "smtlib.h"
#include <map>
#include <sstream>

#define HAVE_PCON 1
#if HAVE_PCON

zval *get_zval(zend_execute_data *zdata, int node_type, const znode_op *node)
{
    zend_free_op should_free;
    zval* r = zend_get_zval_ptr(zdata->opline, node_type, node, zdata, &should_free, BP_VAR_IS);
    return r;
}
void force_ref(zval *val) {
    if (!Z_ISREF_P(val) &&
        (Z_TYPE_P(val) != IS_INDIRECT) &&
        (Z_TYPE_P(val) != IS_CONST) &&
        (Z_TYPE_P(val) != IS_CONSTANT_AST))
    {
        if (Z_TYPE_P(val) == IS_UNDEF) {
            ZVAL_NEW_EMPTY_REF(val);
            Z_SET_REFCOUNT_P(val, 2);
            ZVAL_NULL(Z_REFVAL_P(val));
        }
        else {
            ZVAL_MAKE_REF(val);
        }
    }
}



std::map<void*, void*> ref_to_ref;
std::map<void*, zval*> ref_to_res;
std::map<void*, std::string> ref_to_expr;
smt* smt_sink;





inline void retrieve_ref_by_zv(void* ref, zval*& res){
    if (ref_to_res.find(ref) == ref_to_res.end()){
        if (ref_to_ref.find(ref) == ref_to_ref.end())
            res = nullptr;
        else
            retrieve_ref_by_zv(ref_to_ref[ref], res);
    }
    else
        res =  ref_to_res[ref];
}

void retrieve_ref_by_expr(void* ref, std::string& expr, const std::string& type, unsigned int lo){
    if (ref_to_expr.find(ref) == ref_to_expr.end()){
        if (ref_to_ref.find(ref) == ref_to_ref.end()) {
            std::ostringstream expr_name;
            expr_name << ref;
            smt_sink->declare_const(expr_name.str(), type, lo);
            ref_to_expr[ref] = expr_name.str();
            expr = expr_name.str();
        }
        else
            retrieve_ref_by_expr(ref_to_ref[ref], expr, type, lo);
    }
    else
        expr = ref_to_expr[ref];
}

#define retrieve_ref(ref, l_type, lo) \
    zval* op_res;                         \
    std::string op_expr;                                       \
    retrieve_ref_by_zv(ref, op_res);\
    if (op_res == nullptr)                \
        retrieve_ref_by_expr(ref, op_expr, l_type, lo);
#define __line_number  execute_data->opline->lineno
void handle_assign(zend_execute_data* execute_data,
                   zval* op1,
                   zval* op2,
                   unsigned int& T){
    php_printf("x");
    switch (Z_TYPE_P(op1)) {
        case IS_NULL:
        case IS_UNDEF:
            force_ref(op1);
            handle_assign(execute_data, op1, op2, T);
            return;
        case IS_REFERENCE:
            switch (Z_TYPE_P(op2)) {
                case IS_RESOURCE:
                case IS_OBJECT:
                case IS_ARRAY:
                {
                    std::string op_expr;
                    retrieve_ref_by_expr(Z_RES_VAL_P(op2), op_expr, "Int", __line_number);
                    insert_or_assign_cc17(ref_to_expr, Z_REFVAL_P(op1), op_expr)
                    break;
                }
                case IS_LONG:
                case IS_DOUBLE:
                case IS_TRUE:
                case IS_FALSE:
                case IS_STRING:
                    insert_or_assign_cc17(ref_to_res, Z_REFVAL_P(op1), op2)
                    break;
                case IS_REFERENCE:
                {
                    retrieve_ref(Z_REFVAL_P(op2), "Int", __line_number)
                    if (op_res != nullptr){
                        insert_or_assign_cc17(ref_to_res, Z_REFVAL_P(op1), op_res)
                    } else {
                        insert_or_assign_cc17(ref_to_expr, Z_REFVAL_P(op1), op_expr)
                    }
                    break;
                }
                default:
                    php_printf("UNKNOWN OP: Assign by type %d\n", Z_TYPE_P(op2));
                    break;
            }
            break;
        default:
            php_printf("UNKNOWN OP: Assign to type %d\n", Z_TYPE_P(op1));
            break;
    }
}
#define __is_not_res_op1 op1_res == nullptr
#define __is_not_res_op2 op2_res == nullptr
#define __equal_then_else(op1, op2) \
        if (is_equal)     \
            {op1}           \
        else              \
            {op2}
void handle_equal(zend_execute_data* execute_data,
                   zval* op1,
                   zval* op2,
                   bool is_equal,
                   unsigned int& T) {

    zval *op1_res;
    std::string op1_expr;
    if (Z_TYPE_P(op1) == IS_REFERENCE) {
        retrieve_ref_by_zv(Z_REFVAL_P(op1), op1_res);
        if (__is_not_res_op1) {
            switch (Z_TYPE_P(op2)) {
                case IS_TRUE:
                case IS_FALSE:
                    retrieve_ref_by_expr(Z_REFVAL_P(op1), op1_expr, "Bool", __line_number);
                    break;
                case IS_STRING:
                    retrieve_ref_by_expr(Z_REFVAL_P(op1), op1_expr, "String", __line_number);
                    break;
                default:
                    // todo: add support for op2 ref
                    retrieve_ref_by_expr(Z_REFVAL_P(op1), op1_expr, "Int", __line_number);
                    break;
            }
        }
    } else
        op1_res = op1;

    zval *op2_res;
    std::string op2_expr;
    if (Z_TYPE_P(op2) == IS_REFERENCE) {
        retrieve_ref_by_zv(Z_REFVAL_P(op2), op2_res);
        if (__is_not_res_op2) {
            switch (op1_res == nullptr ? IS_LONG : Z_TYPE_P(op1_res)) {
                case IS_LONG:
                    retrieve_ref_by_expr(Z_REFVAL_P(op2), op2_expr, "Int", __line_number);
                    break;
                case IS_TRUE:
                case IS_FALSE:
                    retrieve_ref_by_expr(Z_REFVAL_P(op2), op2_expr, "Bool", __line_number);
                    break;
                case IS_STRING:
                    retrieve_ref_by_expr(Z_REFVAL_P(op2), op2_expr, "String", __line_number);
                    break;
                default:
                    // todo: add support for op2 ref
                    retrieve_ref_by_expr(Z_REFVAL_P(op2), op2_expr, "Int", __line_number);
                    break;
            }
        }
    } else
        op2_res = op2;

    if (__is_not_res_op1 && !(__is_not_res_op2)) {
        // op1 is expr, op2 is res
        switch (Z_TYPE_P(op2_res)) {
            case IS_LONG:
                __equal_then_else(
                        smt_sink->add_equal(op1_expr, Z_LVAL_P(op2_res), __line_number);,
                        smt_sink->add_nequal(op1_expr, Z_LVAL_P(op2_res), __line_number);
                )
                break;
            case IS_TRUE:
                smt_sink->add_equal(op1_expr, is_equal, __line_number);
                break;
            case IS_FALSE:
                smt_sink->add_equal(op1_expr, !is_equal, __line_number);
                break;
            case IS_RESOURCE:
            case IS_OBJECT:
            case IS_ARRAY: {
                std::string op_expr;
                retrieve_ref_by_expr(Z_RES_VAL_P(op2), op_expr, "Int", __line_number);
                __equal_then_else(
                        smt_sink->add_equal_expr(op1_expr, op_expr, __line_number);,
                        smt_sink->add_equal_expr(op1_expr, op_expr, __line_number);
                )
                break;
            }
            default:
                php_printf("UNKNOWN OP: Unsupported type %d at %d\n", Z_TYPE_P(op2_res), __line_number);
        }
    }

    if (__is_not_res_op1 && __is_not_res_op2){
        // both are expr
        __equal_then_else(
                smt_sink->add_equal_expr(op1_expr, op2_expr, __line_number);,
                smt_sink->add_nequal_expr(op1_expr, op2_expr, __line_number);
        )
    }

    if (!(__is_not_res_op1) && __is_not_res_op2){
        // op1 is res, op2 expr
        return handle_equal(execute_data, op2, op1, is_equal, T);
    }

    if (!(__is_not_res_op1) && !(__is_not_res_op2)){
        // both are res
        switch (Z_TYPE_P(op1_res)) {
            case IS_LONG:
                switch (Z_TYPE_P(op2_res)) {
                    case IS_RESOURCE:
                    case IS_OBJECT:
                    case IS_ARRAY:
                    {
                        // get op2 expr
                        std::string op_expr;
                        retrieve_ref_by_expr(Z_RES_VAL_P(op2), op_expr, "Int", __line_number);
                        __equal_then_else(
                                smt_sink->add_equal(op_expr, Z_LVAL_P(op1_res), __line_number);,
                                smt_sink->add_nequal(op_expr, Z_LVAL_P(op1_res), __line_number);
                        )
                        break;
                    }
                    default:
                        php_printf("Constant value comparison at %d\n", __line_number);
                }
                break;
            case IS_TRUE:
                switch (Z_TYPE_P(op2_res)) {
                    case IS_RESOURCE:
                    case IS_OBJECT:
                    case IS_ARRAY:
                    {
                        // get op2 expr
                        std::string op_expr;
                        retrieve_ref_by_expr(Z_RES_VAL_P(op2), op_expr, "Bool", __line_number);
                        __equal_then_else(
                            smt_sink->add_equal(op_expr, true, __line_number);,
                            smt_sink->add_nequal(op_expr, true, __line_number);
                        )

                        break;
                    }
                    default:
                        php_printf("Constant value comparison at %d\n", __line_number);
                }
                break;
            case IS_FALSE:
                switch (Z_TYPE_P(op2_res)) {
                    case IS_RESOURCE:
                    case IS_OBJECT:
                    case IS_ARRAY:
                    {
                        // get op2 expr
                        std::string op_expr;
                        retrieve_ref_by_expr(Z_RES_VAL_P(op2), op_expr, "Bool", __line_number);
                        __equal_then_else(
                            smt_sink->add_equal(op_expr, false, __line_number);,
                            smt_sink->add_nequal(op_expr, false, __line_number);
                        )
                        break;
                    }
                    default:
                        php_printf("Constant value comparison at %d\n", __line_number);
                }
                break;
            case IS_RESOURCE:
            case IS_OBJECT:
            case IS_ARRAY:
            {
                std::string _op1_expr;
                std::string _op2_expr;
                switch (Z_TYPE_P(op2_res)) {
                    case IS_RESOURCE:
                    case IS_OBJECT:
                    case IS_ARRAY:
                    {
                        // get op2 expr
                        retrieve_ref_by_expr(Z_RES_VAL_P(op2), _op1_expr, "Int", __line_number);
                        retrieve_ref_by_expr(Z_RES_VAL_P(op2), _op2_expr, "Int", __line_number);
                        __equal_then_else(
                                smt_sink->add_equal_expr(_op1_expr, _op2_expr, __line_number);,
                                smt_sink->add_nequal_expr(_op1_expr, _op2_expr, __line_number);
                        )
                        break;
                    }
                    case IS_LONG:
                    case IS_TRUE:
                    case IS_FALSE:
                        return handle_equal(execute_data, op2, op1, is_equal, T);
                    default:
                        php_printf("Constant value comparison at %d\n", __line_number);
                }
                break;
            }
            default:
                php_printf("UNKNOWN OP: Unsupported type %d at %d\n", Z_TYPE_P(op2_res), __line_number);
        }
    }


}


static int conc_collect(zend_execute_data *execute_data)
{
    zval* op1 = get_zval(execute_data, execute_data->opline->op1_type, &execute_data->opline->op1);
    zval* op2 = get_zval(execute_data, execute_data->opline->op2_type, &execute_data->opline->op2);
    zval* result = get_zval(execute_data, execute_data->opline->result_type, &execute_data->opline->result);
    php_printf("================= lo: %d =================\n", execute_data->opline->lineno);

    unsigned int callback_code;
    switch (execute_data->opline->opcode) {
        case ZEND_ASSIGN:
            handle_assign(execute_data, op1, op2, callback_code);
            break;
        case ZEND_IS_EQUAL:
            is_equal_function(result, op1, op2);
            auto is_equal = Z_TYPE_P(result) == IS_TRUE;
            handle_equal(execute_data, op1, op2, is_equal, callback_code);
            break;
    }

    return ZEND_USER_OPCODE_DISPATCH;
}


PHP_MINIT_FUNCTION(pcon)
{
    smt_sink = new smt();
    zend_set_user_opcode_handler(ZEND_IS_EQUAL, conc_collect);
    zend_set_user_opcode_handler(ZEND_ADD, conc_collect);
    zend_set_user_opcode_handler(ZEND_ASSIGN, conc_collect);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(pcon)
{
//    Z3_del_context(ctx);
    delete smt_sink;
    return SUCCESS;
}


/* Argument info for each function, used for reflection */
ZEND_BEGIN_ARG_INFO_EX(arginfo_pcon_nop, 0, 1, 0)
    ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
ZEND_END_ARG_INFO()

/* Add all functions. (Keep PHP_FE_END as last element) */
static const zend_function_entry functions[] = {
    PHP_FE(pcon_nop, arginfo_pcon_nop)
    PHP_FE_END
};

zend_module_entry pcon_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_PCON_EXTNAME,
    functions,
    PHP_MINIT(pcon),
    PHP_MSHUTDOWN(pcon),
    NULL,
    NULL,
    NULL,
    PHP_PCON_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_PCON
extern "C" {
    ZEND_GET_MODULE(pcon)
}
#endif

/* Replace the example function with something better :) */
PHP_FUNCTION(pcon_nop)
{
    zend_string *str;
    ZEND_PARSE_PARAMETERS_START_EX(ZEND_PARSE_PARAMS_THROW, 1, 1)
        Z_PARAM_STR(str)
    ZEND_PARSE_PARAMETERS_END();
    php_printf("2\n");

    RETVAL_STR(str);
}


#endif
