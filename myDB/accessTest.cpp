#include <cstdio>
#include <cstring>
#include <iostream>
#include "accessControl.h"
#include "interpreter.h"

using namespace std;


void testGrantRevoke();

void testDbAccess();

int mainMK() {

    //testGrantRevoke();
    //testDbAccess();

    Access access;
    Interpreter interpreter;
    table_opration action;
    fileUtil fileTools;
    access.checkLogin("root", "123456");
    // testDbAccess();
    access.currentDbName = "EX";
    string sql[7];
    sql[0] = "create table test(name char(20),pass char(20));";
    sql[1] = "DROP TABLE test;";
    sql[2] = "select * from test;";
    sql[3] = "update test set select=1 where userName=1;";
    sql[4] = "insert into test values(ttt,3456);";
    sql[5] = "delete from test where userName=ttt;";


    for (int i = 0; i < 6; i++) {
        vector<string> params = interpreter.analyse(sql[i]);
        bool a = access.checkTbAccess(params);
        cout << a << endl;
    }

    return 0;
}

void testDbAccess() {

    Access access;
    Interpreter interpreter;
    table_opration action;
    fileUtil fileTools;
    access.checkLogin("root", "123456");

    string sql[4] = {"DROP DATABASE EX1X;", "CREATE DATABASE sys;", "DROP DATABASE SYS;"};
    for (int i = 0; i < 3; i++) {
        vector<string> params = interpreter.analyse(sql[i]);
        int a = access.checkDbAccess(params);
        cout << i << a << endl;
    }

    return;
}

void testGrantRevoke() {

    Access access;
    Interpreter interpreter;
    table_opration action;
    fileUtil fileTools;


    // update test
    string pa[5] = {"select", "update", "insert", "delete"};
    for (int i = 0; i < 4; i++) {
        string sql = "GRANT " + pa[i] + " ON tbName TO user;";
        vector<string> vec = interpreter.analyse(sql);
        access.grantAccess(vec);
    }


    // revoke test
    // string pa[5] = {"select", "update", "insert","delete"};
    for (int i = 0; i < 4; i++) {
        string sql = "REVOKE " + pa[i] + " ON tbName TO user;";
        vector<string> vec = interpreter.analyse(sql);
        access.revokeAccess(vec);
    }

}