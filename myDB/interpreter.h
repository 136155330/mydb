//
// Created by dell on 2018/10/20.
//

#ifndef MYDB_INTERPRETER_H
#define MYDB_INTERPRETER_H


#include <vector>
#include <string>

using std::string;
using std::vector;

class Interpreter {
public:
    Interpreter() {};

    ~Interpreter() {};

    /**
     * @param string sql
     * @return vector<string>
     * 传入sql语句，传出一个string类型的动态数组params，存储着解析后的参数列表
     * params[0]表示sql命令类型，然后params[1],params[2], ...依次表示各个参数
     */
    vector<string> analyse(string sql);

};

#endif //MINIMYSQL_INTERPRETER_H


