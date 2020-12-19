//
// Created by CurtisChen on 2018/11/8.
//

#ifndef MYDB2_ACCESSCONTROL_H
#define MYDB2_ACCESSCONTROL_H

#include <cstring>
#include <iostream>
#include <vector>
#include "interpreter.h"
#include "DB.h"
#include"tableService.h"
#include "tool.h"
#include "fileUtil.h"


using std::string;
using std::vector;

class Access {


public:
    string userName = ""; //登录成功后，记录当前用户名
    string currentDbName = ""; //记录当前使用的数据库名
    Interpreter interpreter;  //语法解析
    table_opration action;
    DB accessDB;           //权限数据库sys
    fileUtil fileTools;

public:
    Access();

    ~Access();


    /**
     * 创建用户
     * @param userName
     * @param userPass
     * @return
     */
    bool createUser(string userName, string userPass);


    /**
     * 检测登录
     * @param userName
     * @param userPass
     * @return
     */
    bool checkLogin(string userName, string userPass);

    /**
     * 查询当前用户是否有相应的权限
     * @param params
     * @param currentDB
     * @return
     */
    bool checkAccess(vector<string> params, DB *currentDB);

    /**
     * 查询当前用户是否有对库操作的权限
     * @param params
     * @return
     */
    bool checkDbAccess(vector<string> params);

    /**
     * 查询当前用户是否有对表操作的权限
     * @param params
     * @return
     */
    bool checkTbAccess(vector<string> params);

    /**
     * 赋予权限
     * @param params
     * @return
     */
    bool grantAccess(vector<string> params);

    /**
     * 收回权限
     * @param params
     * @return
     */
    bool revokeAccess(vector<string> params);


};


#endif //MYDB2_ACCESSCONTROL_H
