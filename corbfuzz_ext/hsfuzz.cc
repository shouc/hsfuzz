/*
  +----------------------------------------------------------------------+
  | PHP extension for Coverage Bitmap                                    |
  +----------------------------------------------------------------------+
  | Copyright (c) 2020                                                   |
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
  | Author: Chaofan Shou <shou@ucsb.edu>                                 |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_hsfuzz.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include <map>
#include <sstream>
#include <fstream>

#define HAVE_HSFUZZ 1
#if HAVE_HSFUZZ

zval *get_zval(zend_execute_data *zdata, int node_type, const znode_op *node)
{
    zend_free_op should_free;
    zval* r = zend_get_zval_ptr(zdata->opline, node_type, node, zdata, &should_free, IS_VAR);
    return r;
}
std::map<size_t, size_t> hash_map;
#define line_number execute_data->opline->lineno
#define bm_loc hash_map[_hash]
#define b_comp(func) \
    {                \
        func(result, op1, op2);\
        auto is_equal = Z_TYPE_P(result) == IS_TRUE; \
        bm_loc += 1; \
        break;\
    }
#define MAX_DEPTH 10
unsigned int count_slash(char* path){
    unsigned int count = 0;
    char* curr_path(path);
    while (*curr_path != 0){
        if (*curr_path == '/')
            count++;
        curr_path++;
    }
    return count;
}

void to_file_path(char* path){
    char* curr_path(path);
    char* last_slash(path);
    while (*curr_path != 0){
        if (*curr_path == '/')
            last_slash = curr_path;
        curr_path++;
    }
    *last_slash = 0;
}

unsigned int FP_SLASH_COUNT;

bool is_too_deep(char* path){
    return imaxabs(count_slash(path) - FP_SLASH_COUNT) > MAX_DEPTH;
}

size_t hash(const char* p, size_t s) {
    size_t result = 0;
    const size_t prime = 17;
    for (size_t i = 0; i < s; ++i) {
        result = p[i] + (result * prime);
    }
    return result;
}
#define CORBFUZZ_MAGIC_NUM 1333333337
#define corbfuzz_ht arr
#define corbfuzz_hav corbfuzz_ht->arData->val
bool is_corbfuzz_query(zval* x){
    if (Z_TYPE_P(x) == IS_ARRAY){
        auto arr = Z_ARR_P(x);
        if (Z_TYPE(corbfuzz_hav) == IS_LONG && corbfuzz_hav.value.lval == CORBFUZZ_MAGIC_NUM){
            return true;
        }
    }
    return false;
}

inline std::string get_query_str(zval* symbolic_var) {
    auto arr = Z_ARR_P(symbolic_var);
#define query_zv arr->arData[1].val
    auto query = Z_STRVAL(query_zv);
    auto query_len = Z_STRLEN(query_zv);
    auto query_str = new char[query_len + 1];
    memcpy(query_str, query, query_len);
    query_str[query_len] = '\0';
    std::string _query_str(query_str);
    delete[] query_str;
    return _query_str;
}

#define decl_handler(t) void t(zval* symbolic_var)
#define decr_ref symbolic_var->value.counted->gc.refcount--;

decl_handler(do_int){
    decr_ref;
    symbolic_var->value.arr->arData[1].val.value.counted->gc.refcount--;
    symbolic_var->value.arr->arData[2].val.value.counted->gc.refcount--;
    ZVAL_LONG(symbolic_var, 1);
}

decl_handler(do_double){

}

decl_handler(do_null){

}
decl_handler(do_bool){

}
decl_handler(do_string){

}

void synthesize(zval* symbolic_var, zval* concrete_val){
//    php_printf("Handling CorbFuzz Synthesis\n");
    switch (Z_TYPE_P(concrete_val)) {
        case IS_LONG:
            return do_int(symbolic_var);
        case IS_DOUBLE:
            return do_double(symbolic_var);
        case IS_UNDEF:
            return do_null(symbolic_var);
        case IS_FALSE:
        case IS_TRUE:
            return do_bool(symbolic_var);
        case IS_STRING:
            return do_string(symbolic_var);
        case IS_REFERENCE:
            return synthesize(symbolic_var, Z_REFVAL_P(concrete_val));
    }
}

static int conc_collect(zend_execute_data *execute_data)
{
    // collect coverage
    if (EX(func)->op_array.filename == nullptr ||
        !is_too_deep(ZSTR_VAL(EX(func)->op_array.filename))){
        zval* op1 = get_zval(execute_data, execute_data->opline->op1_type, &execute_data->opline->op1);
        zval* op2 = get_zval(execute_data, execute_data->opline->op2_type, &execute_data->opline->op2);
        zval* result = get_zval(execute_data, execute_data->opline->result_type, &execute_data->opline->result);
        // data synthesis
        auto is_op1_sym = is_corbfuzz_query(op1);
        auto is_op2_sym = is_corbfuzz_query(op2);
        if (is_op1_sym && is_op2_sym){

        } else if (is_op1_sym){
            synthesize(op1, op2);
        } else if (is_op2_sym){
            synthesize(op2, op1);
        }



        auto file_name = EX(func)->op_array.filename;
        char hash_inp[1 << 12]; int hash_inp_len;
        if (file_name == nullptr){
            hash_inp_len = sprintf(hash_inp, "NF@%d", line_number);
        } else {
            hash_inp_len = sprintf(hash_inp, "%s@%d", ZSTR_VAL(file_name), line_number);
        }
        auto _hash = hash(hash_inp, hash_inp_len);

        switch (execute_data->opline->opcode) {
            case ZEND_IS_EQUAL:
            case ZEND_IS_NOT_EQUAL:
            case ZEND_CASE:
            b_comp(is_equal_function)
            case ZEND_IS_IDENTICAL:
            case ZEND_IS_NOT_IDENTICAL:
            b_comp(is_identical_function)
            case ZEND_IS_SMALLER:
            b_comp(is_smaller_function)
            case ZEND_IS_SMALLER_OR_EQUAL:
            b_comp(is_smaller_or_equal_function)
            case ZEND_SPACESHIP:
                // seems no one is using this esoteric bla....
                break;
        }
    }
    return ZEND_USER_OPCODE_DISPATCH;
}


PHP_MINIT_FUNCTION(hsfuzz)
{
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hsfuzz)
{
    return SUCCESS;
}

char COV_FILE_NAME[1 << 12];
PHP_RINIT_FUNCTION(hsfuzz)
{
    hash_map.clear();
    zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
    zend_is_auto_global(server_str);
    auto carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"));
    auto file_path = zend_hash_str_find(Z_ARRVAL_P(carrier),
                                          "SCRIPT_FILENAME",
                                          sizeof("SCRIPT_FILENAME") - 1);
    char* file_path_s = Z_STRVAL_P(file_path);
    FP_SLASH_COUNT = count_slash(file_path_s);
    auto cov_file_loc = zend_hash_str_find(Z_ARRVAL_P(carrier),
                                        "HTTP_COV_LOC",
                                        sizeof("HTTP_COV_LOC") - 1);

    to_file_path(file_path_s);

    if (cov_file_loc == nullptr){
        time_t t = time(nullptr);
        sprintf(COV_FILE_NAME, "%s/cov/%ld.txt", file_path_s, t);
    } else {
        sprintf(COV_FILE_NAME, "%s/cov/%s.txt", file_path_s, Z_STRVAL_P(cov_file_loc));
    }

    zend_set_user_opcode_handler(ZEND_IS_EQUAL, conc_collect);
    zend_set_user_opcode_handler(ZEND_IS_NOT_EQUAL, conc_collect);
    zend_set_user_opcode_handler(ZEND_CASE, conc_collect);
    zend_set_user_opcode_handler(ZEND_IS_IDENTICAL, conc_collect);
    zend_set_user_opcode_handler(ZEND_IS_NOT_IDENTICAL, conc_collect);
    zend_set_user_opcode_handler(ZEND_IS_SMALLER, conc_collect);
    zend_set_user_opcode_handler(ZEND_IS_SMALLER_OR_EQUAL, conc_collect);
    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(hsfuzz)
{
    std::ofstream cov_file;
    cov_file.open(COV_FILE_NAME);
    cov_file << '{';
    for (auto it : hash_map)
        cov_file << it.first << ":" << it.second << ",";
    cov_file << '}';
    cov_file.close();
    return SUCCESS;
}


zend_module_entry hsfuzz_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_HSFUZZ_EXTNAME,
    NULL,
    PHP_MINIT(hsfuzz),
    PHP_MSHUTDOWN(hsfuzz),
    PHP_RINIT(hsfuzz),
    PHP_RSHUTDOWN(hsfuzz),
    NULL,
    PHP_HSFUZZ_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HSFUZZ
extern "C" {
    ZEND_GET_MODULE(hsfuzz)
}
#endif

#endif
