#include <fstream>
#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include"interpreter.h"
#include"DB.h"
#include "fileUtil.h"
#include"tableService.h"
#include"tool.h"
#include "view.h"
#include "viewOperation.h"
#include "accessControl.h"

#define  ROOT "D://allDB"
using namespace std;
DB *currentDB;
vector<DB> DBlist;   //可以有缓存的作用
table_opration action;
fileUtil fileTools;
Access myAccess;

int miniMysql();

bool checkCurrentDB();


int main() {


    while (true) {
        cout << "*****************************************" << endl;
        cout << "              MyDBMS" << endl;
        cout << "*****************************************" << endl;
        int choice = 0;
        cout << "             1 登录" << endl;
        cout << "             2 创建用户" << endl;
        cout << "             3 退出系统" << endl;
        cout << "请选择: ";
        cin >> choice;
        getchar();
        if (choice == 1) {
            string userName, userPass;
            cout << "请输入用户名: ";
            cin >> userName;
            cout << "请输入密码: ";
            cin >> userPass;

            if (myAccess.checkLogin(userName, userPass)) {
                miniMysql();
            }

        } else if (choice == 2) {
            string userName, userPass;
            cout << "请输入用户名：";
            cin >> userName;
            cout << "请输入密码: ";
            cin >> userPass;

            myAccess.createUser(userName, userPass);//创建用户
        } else if (choice == 3) {

            break;
        } else {
            cout << "输入错误，请重新输入！" << endl;
        }
    }

    return 0;
}


int miniMysql() {
    string sql;
    Interpreter interpreter; //SQL解析
    getchar();
    while (true) {
        cout << endl << "sql>";
        getline(cin, sql);

        ///语法解析
        vector<string> params = interpreter.analyse(sql);

        ////判读sql语句是否正确
        if (params.size() == 0) {  //没有匹配到任何字符
            cout << "输入的语句有错误" << endl;
            continue;
        }

        ///权限控制,查询当前用户是否有相应的执行权限
        if (!myAccess.checkAccess(params, currentDB)) {
            continue;
        }

        ///库操作语句
        if (params[0] == "SHOW_DB") {
            vector<string> DBName = fileTools.getDir("D://allDB");
            for (int i = 0; i < DBName.size(); i++) {
                cout << DBName[i] << " ";
            }
            continue;
        }

        if (params[0] == "USE_DB") {
            DB tempDB = DB();
            if (!fileTools.readDB(&tempDB, params[1])) {
                cout << "数据库不存在";
                continue;
            }
            DBlist.push_back(tempDB);
            currentDB = &DBlist[DBlist.size() - 1];
            cout << "成功切换到" << params[1];
        }

        if (params[0] == "CREATE_DB") {
            DB newDB;
            newDB.name = params[1];
            DBlist.push_back(newDB);
            //if database exit failed
            if (fileTools.inDir(ROOT, params[1])) {
                cout << "数据库已存在";
                continue;
            }
            if (!fileTools.writeDB(newDB)) {  //写入文件
                cout << "writeDB error";
                continue;
            }
            cout << "成功创建数据库" << params[1] << endl;
            continue;
        }

        if (params[0] == "DROP_DB") {
            string dbName = params[1];
            string path = ROOT;
            path = path + "//" + dbName;
            if (currentDB != NULL && tool::compareNoCase(dbName, currentDB->name)) {
                //销毁空间
                delete currentDB;
                currentDB = NULL;
            }
            fileTools.dropPath(path);
            cout << "删除成功" << endl;
            continue;
        }

        if (params[0] == "COMMIT") {  //把缓存在DBlist中数据全部的写入文件
            for (int i = 0; i < DBlist.size(); i++) {
                if (!fileTools.writeDB(*currentDB)) {
                    cout << "writeDB error" << endl;
                }
            }
            continue;
        }

        if (params[0] == "EXIT") {
            break;
        }


        ///先检查是否已经选择数据库，再进行表的操作
        if (!checkCurrentDB()) {
            continue;
        }

        ///表操作语句
        if (params[0] == "SHOW_TB") {
            vector<string> tableName = currentDB->showTables();
            if (tableName.empty()) {
                cout << "当前数据库没有表" << endl;
                continue;
            }
            for (int i = 0; i < tableName.size(); i++) {
                cout << tableName[i] << " ";

            }
            continue;
        }

        if (params[0] == "CREATE_TABLE") {
            vector<string> reOrder;
            Table table;
            string path(ROOT);
            if (fileTools.inDir(path + "//" + currentDB->name, params[1])) {
                cout << "表已存在";
                continue;
            }
            reOrder.assign(params.begin() + 1, params.end());  //删除到第一个 标志语
            if (!table.Create(reOrder)) { //表格创建失败
                continue;
            }
            //把建好的table加入到当前数据库 currentDB
            currentDB->addTable(table);
            cout << params[1] << "创建成功" << endl;
        }

        if (params[0] == "DELETE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //去除掉DELETE
            int count = action.Delete(message, currentDB);
            if (count != -1) {
                cout << "删除条数为" << count << "条";
            }
        }

        if (params[0] == "INSERT_TABLE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //去除掉INSERT_TABLE
            if (action.Add(message, currentDB)) {
                cout << "插入成功";
            }
            continue;
        }

        if (params[0] == "UPDATE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //去除掉INSERT_TABLE
            int count = action.Update(message, currentDB);
            if (count != -1) {
                cout << "更新了" << count << "数据";
            }
        }

        if (params[0] == "SELECT") {
            vector<string> message;
            vector<int> colMax;
            message.assign(params.begin() + 1, params.end()); //去除掉SELECT
            vector<vector<string>> answer;

            ///判断seLect的对象是视图还是表
            int view_find = 0;
            for (auto i : (*currentDB).views) {
                if (i.name == params[1]) {
                    answer = View_operter::Find(i, message, *currentDB);
                    view_find = 1;
                    break;
                }
            }

            if (view_find == 0)
                answer = action.Find(message, *currentDB);

            if (answer.size() == 0) {
                cout << "空记录";
                continue;
            }
            //获取每列的最大字符串长度
            tool::maxLength(answer, &colMax);

            cout << "--------------------------------------" << endl;
            vector<string> temp = answer[0];
            for (int i = 0; i < temp.size(); i++) {
                cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left); // 用完之后清除
            }
            cout << endl << "--------------------------------------" << endl;
            for (int i = 1; i < answer.size(); i++) {
                vector<string> temp = answer[i];
                for (int i = 0; i < temp.size(); i++) {
                    cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left);
                }
                cout << endl;
            }
            cout << "---------------------------------------" << endl;
        }

        if (params[0] == "DESC_TABLE") {
            string tableName = params[1];
            vector<vector<string>> attriMessage;
            vector<int> colMax; //获取每列最大字符串数，方便格式化输出
            if (!tool::eixtTable(*currentDB, tableName)) {
                cout << "表不存在";
                continue;
            }
            //获取格式化后的字段信息
            attriMessage = tool::descTable(*currentDB, tableName);
            //获取每列的最大字符串长度
            tool::maxLength(attriMessage, &colMax);
            cout << "------------------------------------" << endl;
            vector<string> temp = attriMessage[0];
            for (int i = 0; i < temp.size(); i++) {
                cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left); // 用完之后清除
            }
            cout << endl << "------------------------------------" << endl;
            for (int i = 1; i < attriMessage.size(); i++) {
                vector<string> temp = attriMessage[i];
                for (int i = 0; i < temp.size(); i++) {
                    cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left);
                }

                cout << endl;
            }
            cout << "------------------------------------" << endl;
        }

        if (params[0] == "DROP_TB") {
            string tableName = params[1];
            string path = ROOT;
            path = path + "//" + currentDB->name + "//" + tableName;
            currentDB->dropTable(tableName);
            //文件上删除
            fileTools.dropPath(path);
            cout << "删除成功";
        }

        ///视图操作

        if (params[0] == "CREATE_VIEW") {

            vector<string> message;

            message.assign(params.begin() + 2, params.end()); ///去除掉SELECT

            view new_view(params[1], message);

            (*currentDB).addView(new_view);

            auto temp = new fileUtil();

            temp->createView(currentDB, new_view);

            if (temp) delete temp;

            continue;

        }

        if (params[0] == "DROP_VIEW") {

            (*currentDB).deleteView(params[1]);
            auto temp = new fileUtil();
            temp->deleteView(currentDB, params[1]);
            if (temp) delete temp;
            cout << "删除视图成功" << endl;
            continue;

        }


    }

    return 0;
}


bool checkCurrentDB() {
    if (currentDB == NULL) {
        vector<string> DBName = fileTools.getDir("D://allDB");
        cout << "请选择数据库" << endl;
        for (int i = 0; i < DBName.size(); i++) {
            cout << DBName[i] << " ";
        }
        return false;
    } else {
        return true;
    }
}
