//
// Created by dell on 2018/10/24.
//

#ifndef MYDB_TOOL_H
#define MYDB_TOOL_H

#include<iostream>
#include<string>
#include<vector>
#include <algorithm> //实现find方法
#include "DB.h"

using namespace std;

class tool {
public:
    //把字符串转为大写
    static string strToUpper(string str);

    //忽略大小写比较string
    static bool compareNoCase(string strA, string strB);

    //按一个字符分割一个字符串
    static int split(const string &str, vector<string> &ret_, char sep);

    //判断是否有分号结尾
    //获取对应属性的位置,放回查询的所有属性名称
    static vector<string> GetAttriPosition(Table table, vector<string> attriName, vector<int> *positon);

    //增加部分
    //获取value中对应位置的值（position中），并返回
    static vector<string> GetSelectAttri(vector<string> value, vector<int> position);
    //生成一个vector<string>,把value的值存入对应位置 如 value= libai 110 positon=2 4 即把 libai 存入下标为2的位置，把110存入下标为4的位置
    /**
     *
     * @param value    要插入的值
     * @param positon  要插入的位置
     * @param length  规定返回的容器的长度
     * @return    返回一个把value插入对应位置的长度为length容器，其余位置为"null"
     */
    static vector<string> opInsert(vector<string> value, vector<int> positions, int length);

    //去除string 首尾空格
    static string trim(string str);

    /**
     * 判断输入的属性是否是表中的属性，
     * 不是： 输出提示，返回false
     * 是 返回true
     * @param table
     * @param attrs
     * @return
     */
    static bool eixtAttri(Table table, vector<string> attrs);

    /**
     * 判断表格是否存在 //区别于在文件中查询
     * @param currentDB
     * @param tablename
     * @return
     */
    static bool eixtTable(DB currentDB, string tablename);

    /**
     * 获取对应位置的属性类型
     * @param table
     * @param position 属性的位置
     * @return 对应位置属性的值
     */
    static vector<string> getType(Table table, vector<int> position);

    /**
     * 判断type类型是否支持
     * @param type
     * @return
     */
    static bool typeExit(string type);

    /**
     * 比对值和类型是否符合
     * @param value
     * @param type
     * @return
     */
    static bool typeJudge(vector<string> value, vector<string> type);

    /**
     * 返回每一列的最大长度
     * @param values
     * @return
     */
    static void maxLength(vector<vector<string>> values, vector<int> *colMax);

    /**
     * 返回表中所有字段的信息
     * 如：字段名，类型，主键
     * @param tableName
     * @return
     */
    static vector<vector<string>> descTable(DB currentDB, string tableName);

    /**
     * 判断输入的值，是否符合主键，独立性，和空值约束
     * @param value
     * @param postions  value中值对应属性的位置
     * @param table      要判断的表
     * @return
     */
    static bool judgeConstraints(vector<string> value, vector<int> postions, Table table);

    /**
     * 获取第 i 列的所有值
     * @param values
     * @param i
     * @return
     */
    static vector<string> getCol(vector<vector<string>> values, int i);


};

#endif //MYDB_TOOL_H
