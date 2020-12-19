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
     * �������������ʵ�ֿ���ֱ�Ӷ�дAttribute������ļ�
     * @param input
     * @param attri
     * @return
     * ������Ҫע�⣬������������ĺ���Ӧ��Ϊ��Ԫ��������Ϊ��ĳ�Ա��Ԫ���������Ҫ��������������Ϊ����������ĵ�һ������,��������е�>>��Ҫ���һ������Ϊostream������,���Բ�����Ϊ���Ա,ֻ����Ϊ��Ԫ.
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

