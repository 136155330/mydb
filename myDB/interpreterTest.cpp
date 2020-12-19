//
// Created by dell on 2018/10/20.
//

/**
 *
 *  �Խ���ģ��interpreter���е�Ԫ����
 *
 */

#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include "interpreter.h"

using namespace std;

// ��������б�
void outputParas(vector<string> vec);

void testRe();

void testInterpreter() {
    vector<string> vec;
    Interpreter interpreter;
    string sql;
    // 1��Ȩ�����
    //��1��
    sql = "REVOKE SELECT ON db2 TO xiaoming";
    vec = interpreter.analyse(sql);
    outputParas(vec);
    //2
    sql = "GRANT DELETE ON db1 TO xiaox";
    vec = interpreter.analyse(sql);
    outputParas(vec);
    //3����������
    sql = "CREATE DATABASE dbName;";
    vec = interpreter.analyse(sql);
    outputParas(vec);
    //4
    sql = "DROP DATABASE dbName;";
    vec = interpreter.analyse(sql);
    outputParas(vec);
    //5
    sql = "SHOW DATABASES;";
    vec = interpreter.analyse(sql);
    outputParas(vec);
    //6
    sql = "USE dbName;";
    vec = interpreter.analyse(sql);
    outputParas(vec);

    //��������
    // ����


    //�������
    //(1)�ύ���� commit;
    sql = "COMMIT;";
    vec = interpreter.analyse(sql);
    outputParas(vec);

    //(2)�˳� exit;
    sql = "EXIT;";
    vec = interpreter.analyse(sql);
    outputParas(vec);

}


int mainm() {

    vector<string> vec;
    Interpreter interpreter;
    string sql = "DROP VIEW testview;";
    sql = "CREATE VIEW viewxx AS SELECT * FROM tb;";
    vec = interpreter.analyse(sql);
    outputParas(vec);
}

void testRe() {
    regex patten;
    patten = "^COMMIT;";
    string str = "COMMIT;";
    cout << regex_match(str, patten) << endl;
}


void outputParas(vector<string> vec) {
    for (int i = 0; i < vec.size(); i++) {
        cout << vec[i] << ";";
    }
    cout << endl;
}
