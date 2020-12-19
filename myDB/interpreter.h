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
     * ����sql��䣬����һ��string���͵Ķ�̬����params���洢�Ž�����Ĳ����б�
     * params[0]��ʾsql�������ͣ�Ȼ��params[1],params[2], ...���α�ʾ��������
     */
    vector<string> analyse(string sql);

};

#endif //MINIMYSQL_INTERPRETER_H


