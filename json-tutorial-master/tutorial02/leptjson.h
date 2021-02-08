#ifndef LEPTJSON_H__
#define LEPTJSON_H__

/*
 * 加入数字类型
 */
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

/*
 * 只有在类型为LEPT_NUMBER时，n的值才有意义
 * 数字都以double类型的存储
 */
typedef struct {
    double n;
    lept_type type;
}lept_value;

enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    /*
     * 数字太大
     */
    LEPT_PARSE_NUMBER_TOO_BIG
};

int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

double lept_get_number(const lept_value* v);

#endif /* LEPTJSON_H__ */
