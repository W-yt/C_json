#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "leptjson.h"

static int main_ret = 0;
static int test_count = 0;
static int test_pass = 0;

/*
 * 宏的编写技巧：
 * 当宏里面由多于一条语句时，据需要使用do{}while(0)包裹成为一条语句，否则会出现错误
 * 如下：
 *   1）#define M() a(); b()
 *      if (cond)
 *          M();
 *      else
 *          c();
 *    预处理后变为：
 *      if (cond)
 *          a(); b();
 *      // else缺乏对应if（分号会表示上面的if语句已经结束）
 *      else
 *          c();
 *   2）#define M() { a(); b(); }
 *    预处理后变为：
 *      if (cond)
 *          { a(); b(); };
 *      //最后的分号也会代表if语句的结束
 *      else
 *          c();
 * 而使用do while就可以避免这个问题
 * 这里do while的作用仅限于将语句整合为一个完整的模块，并不用于循环，实际语句仍是顺序执行一次
 */
#define EXPECT_EQ_BASE(equality, expect, actual, format) \
    do {                                                 \
        test_count++;\
        if (equality)\
            test_pass++;\
        else {\
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual);\
            main_ret = 1;\
        }\
    } while(0)
    /*
     * 两个format作为expect和actual的输出格式字符——输出十进制整数
     * 前面的一个%s一个%d分别是错误发生的文件名和行号的格式化字符
     */

    /*
     * 上面的test_count和test_pass两个计数值分别统计测试程序的执行次数以及测试的通过次数
     */

#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")

/*
 * 测试解析null
 */
static void test_parse_null() {
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "null"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}

/*
 * 测试解析true
 */
static void test_parse_true() {
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "true"));
    EXPECT_EQ_INT(LEPT_TRUE, lept_get_type(&v));
}

/*
 * 测试解析false
 */
static void test_parse_false() {
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_OK, lept_parse(&v, "false"));
    EXPECT_EQ_INT(LEPT_FALSE, lept_get_type(&v));
}

/*
 * 测试解析空白
 */
static void test_parse_expect_value() {
    lept_value v;

    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, ""));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));

    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_EXPECT_VALUE, lept_parse(&v, " "));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}

/*
 * 测试解析不存在的字符值
 */
static void test_parse_invalid_value() {
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, lept_parse(&v, "nul"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));

    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_INVALID_VALUE, lept_parse(&v, "?"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}

/*
 * 测试解析实际值后的空白后还有其他字符
 */
static void test_parse_root_not_singular() {
    lept_value v;
    v.type = LEPT_FALSE;
    EXPECT_EQ_INT(LEPT_PARSE_ROOT_NOT_SINGULAR, lept_parse(&v, "null x"));
    EXPECT_EQ_INT(LEPT_NULL, lept_get_type(&v));
}

/*
 * 测试主程序
 */
static void test_parse() {
    test_parse_null();
    test_parse_true();
    test_parse_false();
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_root_not_singular();
}

int main() {
    test_parse();
    /*
     * 输出测试结果统计情况
     */
    printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
    return main_ret;
}
