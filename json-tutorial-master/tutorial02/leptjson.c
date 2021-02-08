#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <errno.h>   /* errno, ERANGE */
#include <math.h>    /* HUGE_VAL */
#include <stdlib.h>  /* NULL, strtod() */

#define EXPECT(c, ch)       do { assert(*c->json == (ch)); c->json++; } while(0)
#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')

typedef struct {
    const char* json;
}lept_context;

static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_true(lept_context* c, lept_value* v) {
    EXPECT(c, 't');
    if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c, lept_value* v) {
    EXPECT(c, 'f');
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    EXPECT(c, 'n');
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
}

/*
 * 合并上面的三个解析函数为一个（解析文字）（练习一）
 */
/*static int lept_parse_literal(lept_context* c, lept_value* v){
    switch (*c->json) {
        case 't':
            if (c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
                return LEPT_PARSE_INVALID_VALUE;
            c->json += 3;
            v->type = LEPT_TRUE;
            return LEPT_PARSE_OK;
        case 'f':
            if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
                return LEPT_PARSE_INVALID_VALUE;
            c->json += 4;
            v->type = LEPT_FALSE;
            return LEPT_PARSE_OK;
        case 'n':
            if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
                return LEPT_PARSE_INVALID_VALUE;
            c->json += 3;
            v->type = LEPT_NULL;
            return LEPT_PARSE_OK;
        default:
            return LEPT_PARSE_INVALID_VALUE;
    }
}*/

/*
 * 合并三个解析函数（练习一）（答案中的实现方式）
 */
static int lept_parse_literal(lept_context* c, lept_value* v, const char* literal, lept_type type) {
    size_t i;
    EXPECT(c, literal[0]);
    for (i = 0; literal[i + 1]; i++)
        if (c->json[i] != literal[i + 1])
            return LEPT_PARSE_INVALID_VALUE;
    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

/*
 * 在该函数中完成Json数字文本的校核功能（练习三）
 */
static int lept_parse_number(lept_context* c, lept_value* v) {
    const char* p = c->json;
    /* 负号 */
    if (*p == '-') p++;
    /* 整数 */
    if (*p == '0') p++;
    else {
        /* 第一个数字如果不是1-9则直接返回错误码 */
        if (!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        /* 跳过剩下的整数数字 */
        for (p++; ISDIGIT(*p); p++);
    }
    /* 小数 */
    if (*p == '.') {
        /* 跳过小数点 */
        p++;
        /* 小数点后至少有一个数字，若没有则直接返回错误码 */
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        /* 跳过剩下的小数数字 */
        for (p++; ISDIGIT(*p); p++);
    }
    /* 指数 */
    if (*p == 'e' || *p == 'E') {
        /* 跳过e/E */
        p++;
        /* 跳过正负号 */
        if (*p == '+' || *p == '-') p++;
        /* 指数部分至少有一个数字，若没有则直接返回错误码 */
        if (!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        /* 跳过剩下的指数数字 */
        for (p++; ISDIGIT(*p); p++);
    }
    /* 执行到这里说明没有格式错误 */
    errno = 0;

    /*
     * C库函数 double strtod(const char *str, char **endptr)
     * 把参数 str 所指向的字符串转换为一个浮点数（类型为 double 型）
     * 如果 endptr 不为空，则指向转换中最后一个字符后的字符的指针会存储在 endptr 引用的位置
     */
    v->n = strtod(c->json, NULL);
    /*
     * 处理数字过大的情况，根据strtod函数的返回值
     * 这里的errno为ERANGE的情况是在哪里呢？
     * C库宏ERANGE表示一个范围错误，它在输入参数超出数学函数定义的范围时发生，errno被设置为ERANGE
     */
    if (errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL))
        return LEPT_PARSE_NUMBER_TOO_BIG;
    /* 运行的到这一步说明转换是成功的，为type赋值为LEPT_NUMBER */
    v->type = LEPT_NUMBER;
    c->json = p;
    return LEPT_PARSE_OK;
}

/*
 * 重构函数lept_parse_literal之后
 * lept_parse_value函数也需要修改 主要是函数调用的部分
 */
static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 't':  return lept_parse_literal(c, v, "true", LEPT_TRUE);
        case 'f':  return lept_parse_literal(c, v, "false", LEPT_FALSE);
        case 'n':  return lept_parse_literal(c, v, "null", LEPT_NULL);
        default:   return lept_parse_number(c, v);
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
    }
}

int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if ((ret = lept_parse_value(&c, v)) == LEPT_PARSE_OK) {
        lept_parse_whitespace(&c);
        if (*c.json != '\0') {
            v->type = LEPT_NULL;
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
        }
    }
    return ret;
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v) {
    /*
     * 用断言确保函数调用正确（类型为LEPT_NUMBER的Json对象才可以获取数值）
     */
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->n;
}
