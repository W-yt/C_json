#include "leptjson.h"
#include <assert.h>  /* assert() */
#include <stdlib.h>  /* NULL */

/*
 * c->json是一个字符指针
 * 确认其指向的字符是n后，就令其指向下一个字符
 */
#define EXPECT(c, ch)   do { assert(*c->json == (ch)); c->json++; } while(0)

/*
 * 为了减少解析函数之间传递多个参数，把这些数据都放进一个lept_context结构体
 */
typedef struct {
    const char* json;
}lept_context;

/*
 * 跳过前面的空白字符
 * 可能是：
 *      “  ”（空格）；
 *      “\t”（制表符tab）；
 *      “\n”（换行符）；
 *      “\r”（回车）
 * 这个函数不会出现错误，所以返回值为void
 */
static void lept_parse_whitespace(lept_context* c) {
    const char *p = c->json;
    while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;
    c->json = p;
}

static int lept_parse_null(lept_context* c, lept_value* v) {
    /*
     * 断言的方式跳过第一个字符n
     */
    EXPECT(c, 'n');
    /*
     * 如果发现内容不是null
     */
    if (c->json[0] != 'u' || c->json[1] != 'l' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    /*
     * 跳过余下的3个字符
     */
    c->json += 3;
    /*
     * 核实后对type正式赋值
     */
    v->type = LEPT_NULL;
    return LEPT_PARSE_OK;
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
    if (c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's'|| c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

/*
 * 处理Json的值实际值部分
 * 但是这里只处理了null的情况
 * 没有处理true和flase的情况（这是练习部分，我已经自己补全了，并添加了两个类似lept_parse_null的判断函数）
 */
static int lept_parse_value(lept_context* c, lept_value* v) {
    switch (*c->json) {
        case 'n': return lept_parse_null(c, v);
        case 't': return lept_parse_true(c, v);
        case 'f': return lept_parse_false(c, v);
        /*
         * \0是字符串结束标志
         * 说明去除前面的空白之后，字符串就结束了
         * 因此这是一个只含有空白的字符串
         */
        case '\0': return LEPT_PARSE_EXPECT_VALUE;
        default:   return LEPT_PARSE_INVALID_VALUE;
    }
}

/*
 * Json解析主函数
 */
int lept_parse(lept_value* v, const char* json) {
    lept_context c;
    /*
     * 断言
     * 可以输出调试信息
     */
    assert(v != NULL);
    /*
     * 将Json文本的内容保存到lept_context中
     */
    c.json = json;
    /*
     * 默认类型为TEPT_NULL（在判断类型之前赋初始值）
     */
    v->type = LEPT_NULL;
    /*
     * 去除Json文本前面的无效字符，空格等
     */
    lept_parse_whitespace(&c);
    /*
     * 实际处理Json文本
     * 其间都是用一个lept_context类型的队形c进行数据的传递
     */
    return lept_parse_value(&c, v);
}

lept_type lept_get_type(const lept_value* v) {
    assert(v != NULL);
    return v->type;
}
