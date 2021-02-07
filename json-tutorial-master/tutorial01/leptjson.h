#ifndef LEPTJSON_H__
#define LEPTJSON_H__

/* 把Json的六种数据类型在一个枚举类型中完成声明，其中把true和false声明成了两个数据类型
 * 该枚举类型命名为lept_type，这里由于C语言没有C++中的命名空间的概念，因此一般会使用项目的简写作为标识符的前缀（lept） */
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

/* 声明Json数据结构 */
typedef struct {
    lept_type type;
}lept_value;

enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

int lept_parse(lept_value* v, const char* json);

lept_type lept_get_type(const lept_value* v);

#endif /* LEPTJSON_H__ */
