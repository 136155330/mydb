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
vector<DB> DBlist;   //�����л��������
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
        cout << "             1 ��¼" << endl;
        cout << "             2 �����û�" << endl;
        cout << "             3 �˳�ϵͳ" << endl;
        cout << "��ѡ��: ";
        cin >> choice;
        getchar();
        if (choice == 1) {
            string userName, userPass;
            cout << "�������û���: ";
            cin >> userName;
            cout << "����������: ";
            cin >> userPass;

            if (myAccess.checkLogin(userName, userPass)) {
                miniMysql();
            }

        } else if (choice == 2) {
            string userName, userPass;
            cout << "�������û�����";
            cin >> userName;
            cout << "����������: ";
            cin >> userPass;

            myAccess.createUser(userName, userPass);//�����û�
        } else if (choice == 3) {

            break;
        } else {
            cout << "����������������룡" << endl;
        }
    }

    return 0;
}


int miniMysql() {
    string sql;
    Interpreter interpreter; //SQL����
    getchar();
    while (true) {
        cout << endl << "sql>";
        getline(cin, sql);

        ///�﷨����
        vector<string> params = interpreter.analyse(sql);

        ////�ж�sql����Ƿ���ȷ
        if (params.size() == 0) {  //û��ƥ�䵽�κ��ַ�
            cout << "���������д���" << endl;
            continue;
        }

        ///Ȩ�޿���,��ѯ��ǰ�û��Ƿ�����Ӧ��ִ��Ȩ��
        if (!myAccess.checkAccess(params, currentDB)) {
            continue;
        }

        ///��������
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
                cout << "���ݿⲻ����";
                continue;
            }
            DBlist.push_back(tempDB);
            currentDB = &DBlist[DBlist.size() - 1];
            cout << "�ɹ��л���" << params[1];
        }

        if (params[0] == "CREATE_DB") {
            DB newDB;
            newDB.name = params[1];
            DBlist.push_back(newDB);
            //if database exit failed
            if (fileTools.inDir(ROOT, params[1])) {
                cout << "���ݿ��Ѵ���";
                continue;
            }
            if (!fileTools.writeDB(newDB)) {  //д���ļ�
                cout << "writeDB error";
                continue;
            }
            cout << "�ɹ��������ݿ�" << params[1] << endl;
            continue;
        }

        if (params[0] == "DROP_DB") {
            string dbName = params[1];
            string path = ROOT;
            path = path + "//" + dbName;
            if (currentDB != NULL && tool::compareNoCase(dbName, currentDB->name)) {
                //���ٿռ�
                delete currentDB;
                currentDB = NULL;
            }
            fileTools.dropPath(path);
            cout << "ɾ���ɹ�" << endl;
            continue;
        }

        if (params[0] == "COMMIT") {  //�ѻ�����DBlist������ȫ����д���ļ�
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


        ///�ȼ���Ƿ��Ѿ�ѡ�����ݿ⣬�ٽ��б�Ĳ���
        if (!checkCurrentDB()) {
            continue;
        }

        ///��������
        if (params[0] == "SHOW_TB") {
            vector<string> tableName = currentDB->showTables();
            if (tableName.empty()) {
                cout << "��ǰ���ݿ�û�б�" << endl;
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
                cout << "���Ѵ���";
                continue;
            }
            reOrder.assign(params.begin() + 1, params.end());  //ɾ������һ�� ��־��
            if (!table.Create(reOrder)) { //��񴴽�ʧ��
                continue;
            }
            //�ѽ��õ�table���뵽��ǰ���ݿ� currentDB
            currentDB->addTable(table);
            cout << params[1] << "�����ɹ�" << endl;
        }

        if (params[0] == "DELETE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //ȥ����DELETE
            int count = action.Delete(message, currentDB);
            if (count != -1) {
                cout << "ɾ������Ϊ" << count << "��";
            }
        }

        if (params[0] == "INSERT_TABLE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //ȥ����INSERT_TABLE
            if (action.Add(message, currentDB)) {
                cout << "����ɹ�";
            }
            continue;
        }

        if (params[0] == "UPDATE") {
            vector<string> message;
            message.assign(params.begin() + 1, params.end()); //ȥ����INSERT_TABLE
            int count = action.Update(message, currentDB);
            if (count != -1) {
                cout << "������" << count << "����";
            }
        }

        if (params[0] == "SELECT") {
            vector<string> message;
            vector<int> colMax;
            message.assign(params.begin() + 1, params.end()); //ȥ����SELECT
            vector<vector<string>> answer;

            ///�ж�seLect�Ķ�������ͼ���Ǳ�
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
                cout << "�ռ�¼";
                continue;
            }
            //��ȡÿ�е�����ַ�������
            tool::maxLength(answer, &colMax);

            cout << "--------------------------------------" << endl;
            vector<string> temp = answer[0];
            for (int i = 0; i < temp.size(); i++) {
                cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left); // ����֮�����
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
            vector<int> colMax; //��ȡÿ������ַ������������ʽ�����
            if (!tool::eixtTable(*currentDB, tableName)) {
                cout << "������";
                continue;
            }
            //��ȡ��ʽ������ֶ���Ϣ
            attriMessage = tool::descTable(*currentDB, tableName);
            //��ȡÿ�е�����ַ�������
            tool::maxLength(attriMessage, &colMax);
            cout << "------------------------------------" << endl;
            vector<string> temp = attriMessage[0];
            for (int i = 0; i < temp.size(); i++) {
                cout << setiosflags(ios::left) << setw(colMax[i] + 3) << temp[i] << resetiosflags(ios::left); // ����֮�����
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
            //�ļ���ɾ��
            fileTools.dropPath(path);
            cout << "ɾ���ɹ�";
        }

        ///��ͼ����

        if (params[0] == "CREATE_VIEW") {

            vector<string> message;

            message.assign(params.begin() + 2, params.end()); ///ȥ����SELECT

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
            cout << "ɾ����ͼ�ɹ�" << endl;
            continue;

        }


    }

    return 0;
}


bool checkCurrentDB() {
    if (currentDB == NULL) {
        vector<string> DBName = fileTools.getDir("D://allDB");
        cout << "��ѡ�����ݿ�" << endl;
        for (int i = 0; i < DBName.size(); i++) {
            cout << DBName[i] << " ";
        }
        return false;
    } else {
        return true;
    }
}
