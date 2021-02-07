#ifndef LEPTJSON_H__
#define LEPTJSON_H__

/*
 * 把Json的六种数据类型在一个枚举类型中完成声明，其中把true和false声明成了两个数据类型
 * 该枚举类型命名为lept_type，这里由于C语言没有C++中的命名空间的概念，因此一般会使用项目的简写作为标识符的前缀（lept）
 */
typedef enum { LEPT_NULL, LEPT_FALSE, LEPT_TRUE, LEPT_NUMBER, LEPT_STRING, LEPT_ARRAY, LEPT_OBJECT } lept_type;

/*
 * 声明Json数据结构
 */
typedef struct {
    lept_type type;
}lept_value;

/*
 * 返回信号值枚举类型定义
 * 只含有空白                 ——>     LEPT_PARSE_EXPECT_VALUE
 * 值之后的空白后面还有其他字符   ——>     LEPT_PARSE_ROOT_NOT_SINGULAR
 * 值不是定义的三种字面值之一    ——>     LEPT_PARSE_INVALID_VALUE
 */
enum {
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR
};

/*
 * 解析Json 传入的Json文本是一个C字符串（const）
 * 一般用法：（传入的Json节点指针是由使用者自行指定的）
 *  lept_value v;
 *  const char json[] = ...;
 *  int ret = lept_parse(&v,json);
 *  没有错误的话，就会返回LEPT_PARSE_OK（= 0）
 */
int lept_parse(lept_value* v, const char* json);

/*
 * 访问结果的函数-获取节点的类型
 */
lept_type lept_get_type(const lept_value* v);

#endif /* LEPTJSON_H__ */
