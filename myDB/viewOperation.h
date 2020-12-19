//
// Created by CurtisChen on 2018/12/3.
//

#ifndef MYDB_VIEW_OPERTER_H
#define MYDB_VIEW_OPERTER_H

#include "DB.h"
#include "view.h"

#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include <io.h>
#include <algorithm> ///find

using std::string;
using std::vector;
using std::fstream;
using std::cout;
using std::endl;
using std::iostream;

class View_operter {
public:

    static bool Judge(view v, vector<string> message_2);

    static vector<vector<string>> Find(view v, vector<string>
    message_2, DB currentDB);


};

#endif //MYDB_VIEW_OPERTER_H
