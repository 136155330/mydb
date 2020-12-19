//
// Created by dell on 2018/10/20.
//

#ifndef _DB_H
#define _DB_H

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include "view.h"

using std::string;
using std::vector;
using std::fstream;
using std::cout;
using std::endl;
using std::iostream;
using namespace std;

class Attribute {
public:
    string name;
    string type;
    bool is_Not_Null;
    bool is_Key;
    bool is_Unique;

    Attribute() {};

    Attribute(string name, string type, string limit);

    bool Judge_exist(vector<Attribute> s);

    /**
     * 重载流运算符，实现可以直接读写Attribute类对象到文件
     * @param input
     * @param attri
     * @return
     * 这里需要注意，重载流运算符的函数应设为友元函数，因为类的成员二元运算符重载要求运算符左操作数为运算符函数的第一个参数,而流类库中的>>则要求第一个参数为ostream的引用,所以不能作为类成员,只能作为友元.
     */
    friend ifstream &operator>>(ifstream &input, Attribute &attri);

    friend ofstream &operator<<(ofstream &output, Attribute &attri);
};

class Table {
public:
    string table_name;
    vector<Attribute> attributes;
    vector<vector<string>> values;

    bool Judge_exist();

    bool Add_attribute(string attribute_name, string attribute_type, string limit);

    bool Create(vector<string> vec);
};

class DB {
public:
    string name;
    vector<Table> tables;
    vector<view> views;

    DB();

    DB(string name);

    void addTable(Table table);

    void addView(view v);

    void deleteView(string viewname);

    vector<string> showTables();

    void dropTable(string tablename);
};

#endif

