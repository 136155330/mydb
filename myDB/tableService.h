//
// Created by CurtisChen on 2018/12/1.
//

#ifndef MYDB_TABLESERVICE_H
#define MYDB_TABLESERVICE_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include "DB.h"
#include "tool.h"   //实现忽略大小写比较string

using namespace std;

typedef vector<vector<string> > tablestruct;

class table_opration {
public:

    bool Add(vector<string> message, DB *currentDB); // message 中有[0]=表名 [1...]=值，DB为当前使用的数据库

    //执行成功返回删除条数，否则放回-1
    int Delete(vector<string> message, DB *currentDB);   //message [0]=表名  [1]=属性名=条件   暂时只支持 and （即所有条件必须全部满足）

    int Update(vector<string> message, DB *currentDB);

    tablestruct Find(vector<string> message, DB currentDB);
};

#endif //MYDB_TABLESERVICE_H
