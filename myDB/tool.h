//
// Created by dell on 2018/10/24.
//

#ifndef MYDB_TOOL_H
#define MYDB_TOOL_H

#include<iostream>
#include<string>
#include<vector>
#include <algorithm> //ʵ��find����
#include "DB.h"

using namespace std;

class tool {
public:
    //���ַ���תΪ��д
    static string strToUpper(string str);

    //���Դ�Сд�Ƚ�string
    static bool compareNoCase(string strA, string strB);

    //��һ���ַ��ָ�һ���ַ���
    static int split(const string &str, vector<string> &ret_, char sep);

    //�ж��Ƿ��зֺŽ�β
    //��ȡ��Ӧ���Ե�λ��,�Żز�ѯ��������������
    static vector<string> GetAttriPosition(Table table, vector<string> attriName, vector<int> *positon);

    //���Ӳ���
    //��ȡvalue�ж�Ӧλ�õ�ֵ��position�У���������
    static vector<string> GetSelectAttri(vector<string> value, vector<int> position);
    //����һ��vector<string>,��value��ֵ�����Ӧλ�� �� value= libai 110 positon=2 4 ���� libai �����±�Ϊ2��λ�ã���110�����±�Ϊ4��λ��
    /**
     *
     * @param value    Ҫ�����ֵ
     * @param positon  Ҫ�����λ��
     * @param length  �涨���ص������ĳ���
     * @return    ����һ����value�����Ӧλ�õĳ���Ϊlength����������λ��Ϊ"null"
     */
    static vector<string> opInsert(vector<string> value, vector<int> positions, int length);

    //ȥ��string ��β�ո�
    static string trim(string str);

    /**
     * �ж�����������Ƿ��Ǳ��е����ԣ�
     * ���ǣ� �����ʾ������false
     * �� ����true
     * @param table
     * @param attrs
     * @return
     */
    static bool eixtAttri(Table table, vector<string> attrs);

    /**
     * �жϱ���Ƿ���� //���������ļ��в�ѯ
     * @param currentDB
     * @param tablename
     * @return
     */
    static bool eixtTable(DB currentDB, string tablename);

    /**
     * ��ȡ��Ӧλ�õ���������
     * @param table
     * @param position ���Ե�λ��
     * @return ��Ӧλ�����Ե�ֵ
     */
    static vector<string> getType(Table table, vector<int> position);

    /**
     * �ж�type�����Ƿ�֧��
     * @param type
     * @return
     */
    static bool typeExit(string type);

    /**
     * �ȶ�ֵ�������Ƿ����
     * @param value
     * @param type
     * @return
     */
    static bool typeJudge(vector<string> value, vector<string> type);

    /**
     * ����ÿһ�е���󳤶�
     * @param values
     * @return
     */
    static void maxLength(vector<vector<string>> values, vector<int> *colMax);

    /**
     * ���ر��������ֶε���Ϣ
     * �磺�ֶ��������ͣ�����
     * @param tableName
     * @return
     */
    static vector<vector<string>> descTable(DB currentDB, string tableName);

    /**
     * �ж������ֵ���Ƿ���������������ԣ��Ϳ�ֵԼ��
     * @param value
     * @param postions  value��ֵ��Ӧ���Ե�λ��
     * @param table      Ҫ�жϵı�
     * @return
     */
    static bool judgeConstraints(vector<string> value, vector<int> postions, Table table);

    /**
     * ��ȡ�� i �е�����ֵ
     * @param values
     * @param i
     * @return
     */
    static vector<string> getCol(vector<vector<string>> values, int i);


};

#endif //MYDB_TOOL_H
