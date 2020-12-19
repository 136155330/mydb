//
// Created by CurtisChen on 2018/12/3.
//
#include "DB.h"
#include "view.h"
#include "viewOperation.h"
#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include <io.h>
#include "tableService.h"
#include <algorithm> ///find


vector<vector<string>> View_operter::Find(view v, vector<string>
message_2, DB currentDB) {
    string flag = "CONDITIONS_START";///这是分割的标志

    vector<string> new_message;

    new_message.push_back(v.message[0]);

    vector<string>::iterator judge = find(v.message.begin
            (), v.message.end(), flag); ///分割地方的迭代器

    vector<string>::iterator judge_2 = find(message_2.begin
            (), message_2.end(), flag);///寻找第二个SELECT 的条件

    vector<string>::iterator it;

    if (v.message[1] != "*" && message_2[1] != "*") {
        if (Judge(v, message_2) == false) {
            vector<string> k;
            k.push_back("error");
            vector<vector<string>> temp;
            temp.push_back(k);
            return temp;
        } else {
            for (it = ++(message_2.begin()); it != judge_2; it++)
                new_message.push_back(*it);
        }

    } else {
        if (v.message[1] == "*" && message_2[1] == "*")
            new_message.push_back("*");
        else if (message_2[1] == "*")
            for (it = ++(v.message.begin()); it != judge; it++)
                new_message.push_back(*it);
        else
            for (it = ++(message_2.begin()); it != judge_2; it++)
                new_message.push_back(*it);
    }

    if (judge == v.message.end() && judge_2 == message_2.end())///说明 原信息 和 查询都没有条件
    {
        table_opration tool;

        return tool.Find(new_message, currentDB);

    } else {
        new_message.push_back("CONDITIONS_START");

        if (judge != v.message.end()) {
            for (it = judge + 1; it != v.message.end(); it++) {
                new_message.push_back(*it);
            }

        }
        if (judge_2 != message_2.end()) {
            for (it = judge_2 + 1; it != message_2.end(); it++) {
                new_message.push_back(*it);
            }
        }

        table_opration tool;

        return tool.Find(new_message, currentDB);
    }
}


bool View_operter::Judge(view v, vector<string> message_2) {
    vector<string>::iterator it, it2;

    for (it2 = (message_2.begin()) + 1; it2 != message_2.end() && *it2 != "CONDITIONS_START"; it2++) {
        int k = 0;
        for (it = (v.message.begin()) + 1; it != v.message.end() && *it != "CONDITIONS_START"; it++) {
            if (*it == *it2) {
                k = 1;
                break;
            }
        }
        if (k == 0)
            return false;
    }

    return true;
}

