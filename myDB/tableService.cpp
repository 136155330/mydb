#include <iostream>
#include <stdio.h>
#include <vector>
#include "DB.h"
#include "tool.h"   //实现忽略大小写比较string
#include "tableService.h"

using namespace std;

typedef vector<vector<string> > tablestruct;


bool table_opration::Add(vector<string> message, DB *currentDB) // message 中有[0]=表名 [1...]=值，DB为当前使用的数据库
{
    //获取message中的表名
    string tableName = message[0];
    vector<string> InsAttri; //存储要插入的字段
    vector<string> value;   //存储要插入的值
    vector<int> positions;  //存储要插入字段的位置
    for (int i = 0; i < currentDB->tables.size(); i++) {    //在DB中查找对应的表
        if (tool::compareNoCase(currentDB->tables[i].table_name, tableName)) {
            //获取要操作的表
            Table *currentTable = &currentDB->tables[i];
            if (message[1] == "") { //插入全部属性 直接插入
                //获取全部属性的位置，即0 1 2 3 4 5 ....
                for (int i = 0; i < currentTable->attributes.size(); i++) {
                    positions.push_back(i);
                }
                //获取插入的值
                value.assign(message.begin() + 3, message.end());
                //判断插入的值和属性的数量是否相同
                if (value.size() != currentTable->attributes.size()) {
                    cout << "值和字段数目不相同";
                    return false;
                }
                //判断插入的值是否类型符合
                if (!tool::typeJudge(value, tool::getType(*currentTable, positions))) {
                    return false;
                }
                //判断是否符合约束条件
                if (!tool::judgeConstraints(value, positions, *currentTable)) {
                    return false;
                }
                //插入数据到currentTable的尾部
                currentTable->values.push_back(value);
                return true;
            } else {  //插入特定字段
                //获取要插入字段名
                string flag = "VALUE_START";
                vector<string>::iterator judge = find(message.begin(), message.end(), flag);  //可以增加异常判断
                for (vector<string>::iterator it = message.begin() + 1; it < judge; it++) {
                    InsAttri.push_back(*it);
                }
                //判断字段是否合法
                if (!tool::eixtAttri(*currentTable, InsAttri)) { //字段不合法
                    return false;
                }
                //获取插入值
                for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                    value.push_back(*it);
                }
                //获取插入的位置
                tool::GetAttriPosition(*currentTable, InsAttri, &positions);
                //判断插入的值和属性的数量是否相同
                if (value.size() != InsAttri.size()) {
                    cout << "值和字段数目不相同";
                    return false;
                }
                //判断插入的值是否类型符合
                if (!tool::typeJudge(value, tool::getType(*currentTable, positions))) {

                    return false;
                }
                //todo
                //判断是否符合约束条件
                if (!tool::judgeConstraints(value, positions, *currentTable)) {
                    return false;
                }
                //生成一个元组
                int length = currentTable->attributes.size();
                vector<string> tempValue = tool::opInsert(value, positions, length);
                currentTable->values.push_back(tempValue);
                return true;
            }
        }
    }
    //表不存在
    cout << "不存在表" << tableName << endl;
    return false;
}

//执行成功返回删除条数，否则放回-1
int
table_opration::Delete(vector<string> message, DB *currentDB)   //message [0]=表名  [1]=属性名=条件   暂时只支持 and （即所有条件必须全部满足）
{
    //获取表名3
    string tableName = message[0];
    int count = 0;  //记录删除的条数
    for (int it = 0; it < currentDB->tables.size(); it++) {
        if (tool::compareNoCase(currentDB->tables[it].table_name, tableName)) {
            //获取要操作的表
            Table *currentTable = &currentDB->tables[it];
            vector<string> attributeName;
            vector<string> requirementValue;     //两个容器分别存储属性名 和 值， 并且一一对应
            //拆分条件
            for (int i = 1; i < message.size(); i++) {
                vector<string> temp;
                tool::split(message[i], temp, '='); // 格式为 name=123456 所以 分割为 name  和 123456
                attributeName.push_back(temp[0]);
                requirementValue.push_back(temp[1]);
            }
            //确定要判断属性的位置
            vector<int> position;
            vector<Attribute> currentAttributes = currentTable->attributes;
            //判断字段是否合法
            if (!tool::eixtAttri(*currentTable, attributeName)) { //字段不合法
                return -1;
            }

            tool::GetAttriPosition(*currentTable, attributeName, &position);
            //寻找满足条件的元组
            for (vector<vector<string>>::iterator it = currentTable->values.begin();
                 it < currentTable->values.end(); it++)  //遍历元组
            {
                bool flag = true;                           //标志值，若flag=true 表示该元组满足条件，否者不满足
                for (int j = 0; j < position.size(); j++)           //判断是否满足所有条件
                {
                    vector<string> currentLine = *it;
                    int point = position[j];
                    if (currentLine[point] != requirementValue[j])   //判断point位置上的字段的值，是否等于条件值
                    {
                        flag = false;   //该元组不满足条件
                        break;        //直接判断下一条元组
                    } //if
                }//for
                if (flag == true) {
                    //删除所有满足条件的元组
                    count += 1;
                    currentTable->values.erase(it);
                    it--;  //删除要保持指针不懂，这样才能指向删除元素的原后继元素
                }
            }//for
            return count;
        }
    }
    //表不存在
    cout << "不存在表" << tableName << endl;
    return -1;
}

int table_opration::Update(vector<string> message, DB *currentDB) {
    string tableName = message[0];
    int count = 0; //记录更新的条数
    for (int it = 0; it < currentDB->tables.size(); it++) {
        if (tool::compareNoCase(currentDB->tables[it].table_name, tableName)) {
            //获取要操作的表
            Table *currentTable = &currentDB->tables[it];
            vector<string> attributeName;
            vector<string> requirementValue;     //两个容器分别存储更新的条件
            vector<string> updateAttribute;
            vector<string> newValue;
            //获取特殊字符的位置，特殊字符之前是更新的值，之后的是判断条件
            string flag = "CONDITIONS_START";
            vector<string>::iterator judge = find(message.begin(), message.end(), flag);  //可以增加异常判断
            vector<string>::iterator start = message.begin();
            //对更新值的拆分
            start += 1;  //跳过前面的表名和提示词
            for (vector<string>::iterator it = start; it < judge; it++) {
                vector<string> temp;
                tool::split(*it, temp, '=');
                updateAttribute.push_back(temp[0]);
                newValue.push_back(temp[1]);
            }
            //对条件的拆分
            for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                vector<string> temp;
                tool::split(*it, temp, '=');  // 格式为 name=123456 所以 分割为 name  和 123456
                attributeName.push_back(temp[0]);
                requirementValue.push_back(temp[1]);
            }
            //判断字段是否合法
            if (!tool::eixtAttri(*currentTable, updateAttribute)) { //更新字段不合法
                return -1;
            }
            if (!tool::eixtAttri(*currentTable, attributeName)) { //判断字段不合法
                return -1;
            }
            //确定条件中要判断属性的位置
            vector<int> position;
            tool::GetAttriPosition(*currentTable, attributeName, &position);
            //确定要更新的属性的位置
            vector<int> updatePosition;
            tool::GetAttriPosition(*currentTable, updateAttribute, &updatePosition);
            //判断对应位置要插入的值是否类型符合
            if (!tool::typeJudge(newValue, tool::getType(*currentTable, updatePosition))) {
                return false;
            }
            //寻找满足条件的元组 table position  requirementValue
            for (int i = 0; i < currentTable->values.size(); i++)  //遍历元组
            {
                bool flag = true;                           //标志值，若flag=true 表示该元组满足条件，否者不满足
                for (int j = 0; j < position.size(); j++)           //判断是否满足所有条件
                {
                    vector<string> currentLine = currentTable->values[i];
                    int point = position[j];
                    if (currentLine[point] != requirementValue[j])   //判断point位置上的字段的值，是否等于条件值
                    {
                        flag = false;   //该元组不满足条件
                        break;        //直接判断下一条元组
                    } //if
                }//for
                if (flag == true) {
                    count += 1;
                    for (int j = 0; j < updatePosition.size(); j++) {
                        vector<string> *currentLine = &currentTable->values[i]; //获取满足条件的元组
                        int point = updatePosition[j];                    //获取更新的位置
                        (*currentLine)[point] = newValue[j];   //更新值
                    }//for
                }
            }//for
            return count;
        }
    }
    //表不存在
    cout << "不存在表" << tableName << endl;
    return -1;
}


tablestruct table_opration::Find(vector<string> message, DB currentDB) {
    string tableName = message[0];
    vector<vector<string>> kong;
    for (int it = 0; it < currentDB.tables.size(); it++) {
        if (tool::compareNoCase(currentDB.tables[it].table_name, tableName)) {
//                cout<<"tableName"<<tableName;
            //获取要操作的表
            Table currentTable = currentDB.tables[it];
            tablestruct tempValues;  //存储满足条件的元组
            vector<string> attributeName;
            vector<string> requirementValue;     //两个容器分别存储属性名 和 值， 并且一一对应
            vector<string> selectAttri;  //用来存储要显示的字段，即select id,name from 中的id,name
            string flag = "CONDITIONS_START";//这是分割的标志
            vector<string>::iterator judge = find(message.begin(), message.end(), flag); //查找分割标志的位置
            if (message[1] == "*") { //若果[1]位置上是* 代表查询全部条件
                vector<string> temp; //temp为了让函数GetAttriPosition，返回所有属性名而设置的零时变量
                temp.push_back("*");
                vector<int> temposition;
                selectAttri = tool::GetAttriPosition(currentTable, temp, &temposition); //传入* ，获取所有字段名
            } else { //不是获取所有字段信息
                //获取要读取的字段名
                for (vector<string>::iterator it = message.begin() + 1;
                     it < judge; it++) //在没有where条件的情况下，judge指向末尾end()
                {
                    selectAttri.push_back(*it);
                }
                //判断表中是存在字段名
                if (!tool::eixtAttri(currentTable, selectAttri)) { //字段不合法
                    return kong;
                }
            }
            tempValues.push_back(selectAttri);   //将要读取的字段名放入tempValues,会被输出显示

            if (judge != message.end())//找到了分割的标志，说明有where条件
            {
                //拆分条件
                for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                    vector<string> temp;
                    tool::split(*it, temp, '=');  // 格式为 name=123456 所以 分割为 name  和 123456
                    attributeName.push_back(temp[0]);
                    requirementValue.push_back(temp[1]);
                }
                //确定要判断属性的位置
                vector<int> position;
                vector<string> currentLine;
                //判断表中是存在字段名
                if (!tool::eixtAttri(currentTable, attributeName)) { //字段不合法
                    return kong;
                }
                tool::GetAttriPosition(currentTable, attributeName, &position);
                vector<Attribute> currentAttributes = currentTable.attributes;
                //寻找满足条件的元组
                for (int i = 0; i < currentTable.values.size(); i++)  //遍历元组
                {
                    bool flag = true;                           //标志值，若flag=true 表示该元组满足条件，否者不满足
                    for (int j = 0; j < position.size(); j++)           //判断是否满足所有条件
                    {
                        currentLine = currentTable.values[i];
                        int point = position[j];
                        if (currentLine[point] != requirementValue[j])   //判断point位置上的字段的值，是否等于条件值
                        {
                            flag = false;   //该元组不满足条件
                            break;        //直接判断下一条元组
                        } //if
                    }//for
                    if (flag == true) {
                        //在这里可以执行对字段的截取操作，以后实现
                        vector<int> positions;     //这两句可以放到for 循环外部，否者会一直定义,即positions的值只需要获得一次
                        tool::GetAttriPosition(currentTable, selectAttri, &positions); //获取属性的位置下标
                        tempValues.push_back(tool::GetSelectAttri(currentLine, positions));     //获取满足条件的元组的特定字段
                    }
                }//for
                if (tempValues.size() == 1) { //当size=1时说明没有满足条件的元组，返回空；
                    return kong;
                } else {
                    return tempValues;
                }
            } else { //没有分割标志，说明没有条件,直接输出表中对应字段的所有信息
                vector<int> positions;
                tool::GetAttriPosition(currentTable, selectAttri, &positions); //获取属性的位置下标
                for (int i = 0; i < currentTable.values.size(); i++) {
                    vector<string> currentLine = currentTable.values[i];
                    tempValues.push_back(tool::GetSelectAttri(currentLine, positions));
                }
                if (tempValues.size() == 1) { //当size=1时说明没有满足条件的元组，返回空；
                    vector<vector<string>> kong;
                    return kong;
                } else {
                    return tempValues;
                }
            }
        }
    }
    //表不存在
    cout << "不存在表" << tableName << endl;
    return kong;
}