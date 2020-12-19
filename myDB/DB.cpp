//
// Created by dell on 2018/10/20.
//


#include "DB.h"
#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include <string.h>
#include <algorithm>  //string 转大写函数
#include"tool.h" //判断表格属性类型是否支持

using std::string;
using std::vector;
using std::fstream;
using std::cout;
using std::endl;
using std::ostream;
using namespace std;


Attribute::Attribute(string name, string type, string limit) {
    this->name = name;
    this->type = type;
    this->is_Key = false;
    this->is_Not_Null = false;
    this->is_Unique = false;

    if (!limit.empty()) {
//            transform(limit.begin(), limit.end(), limit.begin(), ::toupper);
//            if(limit=="PRIMARY KEY")
        if (strcasecmp(limit.c_str(), "PRIMARY KEY") == 0) {
            is_Key = 1;
            is_Not_Null = 1;
            is_Unique = 1;
        }
//            if(limit == "Unique")
        if (strcasecmp(limit.c_str(), "Unique") == 0) {
            is_Unique = 1;
        }
//            if(limit == "Null")
        if (strcasecmp(limit.c_str(), "is not NULL") == 0) {
            is_Not_Null = 1;
        }
    }
}

bool Attribute::Judge_exist(vector<Attribute> s) {
    for (int i = 0; i < s.size(); i++) {
        if (s[i].name == this->name)
            return true;
    }

    return false;
}

ifstream &operator>>(ifstream &input, Attribute &attri) {
    input >> attri.name >> attri.type >> attri.is_Not_Null >> attri.is_Key >> attri.is_Unique;
    return input;


}

ofstream &operator<<(ofstream &output, Attribute &attri) {
    output << attri.name << " " << attri.type << " " << attri.is_Not_Null << " " << attri.is_Key << " "
           << attri.is_Unique << " ";
    return output;
}


bool Table::Judge_exist() {
    fstream file;

    const char *p = table_name.data();

    file.open(p, fstream::in);

    return file.is_open();
}

bool Table::Add_attribute(string attribute_name, string attribute_type, string limit) {

    Attribute a(attribute_name, attribute_type, limit);

    if (a.Judge_exist(attributes))
        return false;

    attributes.push_back(a);

    return true;
}

bool Table::Create(vector<string> vec) //创建表
{
    table_name = vec[0];
    for (int i = 1; i < vec.size(); i += 3) {
        if (!tool::typeExit(vec[i + 1])) {  //类型不支持
            return false;
        }
        if (Add_attribute(vec[i], vec[i + 1], vec[i + 2]) == false) {
            cout << "attribute is exist" << endl;
            return false;
        }
    }
    return true;
}


DB::DB() {
}

DB::DB(string name) {
    this->name = name;
}

void DB::addTable(Table table) {
    //判断表名是否重复
    tables.push_back(table);
}



//视图操作
void DB::addView(view v) {
    views.push_back(v);
}

void DB::deleteView(string viewname) {

    for (auto i = views.begin(); i != views.end(); i++) {
        if ((*i).name == viewname) {
            views.erase(i);
            break;
        }
    }

}


vector<string> DB::showTables() {
    vector<string> allTablesName;
    for (int i = 0; i < this->tables.size(); i++) {
        allTablesName.push_back(this->tables[i].table_name);
    }
    return allTablesName;
}


void DB::dropTable(string tablename) {
    vector<string> allTableName = showTables();
    vector<Table>::iterator it;
    for (it = tables.begin(); it != tables.end(); it++) {
        if (tool::compareNoCase(it->table_name, tablename)) {
            tables.erase(it);
            return;
        }
    }
    //cout << "表不存在";
}


