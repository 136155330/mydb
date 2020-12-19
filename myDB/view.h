//
// Created by CurtisChen on 2018/12/3.
//

#ifndef MYDB_VIEW_H
#define MYDB_VIEW_H

#include<string>
#include<vector>

using std::string;
using std::vector;

class view {
public:
    string name;
    vector<string> message;

    view(string name, vector<string> message);


    bool Create(string name, vector<string> message);

    bool write();

    bool del();

};


#endif //MYDB_VIEW_H
