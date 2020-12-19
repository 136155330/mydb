#include <iostream>
#include <stdio.h>
#include <vector>
#include "DB.h"
#include "tool.h"   //ʵ�ֺ��Դ�Сд�Ƚ�string
#include "tableService.h"

using namespace std;

typedef vector<vector<string> > tablestruct;


bool table_opration::Add(vector<string> message, DB *currentDB) // message ����[0]=���� [1...]=ֵ��DBΪ��ǰʹ�õ����ݿ�
{
    //��ȡmessage�еı���
    string tableName = message[0];
    vector<string> InsAttri; //�洢Ҫ������ֶ�
    vector<string> value;   //�洢Ҫ�����ֵ
    vector<int> positions;  //�洢Ҫ�����ֶε�λ��
    for (int i = 0; i < currentDB->tables.size(); i++) {    //��DB�в��Ҷ�Ӧ�ı�
        if (tool::compareNoCase(currentDB->tables[i].table_name, tableName)) {
            //��ȡҪ�����ı�
            Table *currentTable = &currentDB->tables[i];
            if (message[1] == "") { //����ȫ������ ֱ�Ӳ���
                //��ȡȫ�����Ե�λ�ã���0 1 2 3 4 5 ....
                for (int i = 0; i < currentTable->attributes.size(); i++) {
                    positions.push_back(i);
                }
                //��ȡ�����ֵ
                value.assign(message.begin() + 3, message.end());
                //�жϲ����ֵ�����Ե������Ƿ���ͬ
                if (value.size() != currentTable->attributes.size()) {
                    cout << "ֵ���ֶ���Ŀ����ͬ";
                    return false;
                }
                //�жϲ����ֵ�Ƿ����ͷ���
                if (!tool::typeJudge(value, tool::getType(*currentTable, positions))) {
                    return false;
                }
                //�ж��Ƿ����Լ������
                if (!tool::judgeConstraints(value, positions, *currentTable)) {
                    return false;
                }
                //�������ݵ�currentTable��β��
                currentTable->values.push_back(value);
                return true;
            } else {  //�����ض��ֶ�
                //��ȡҪ�����ֶ���
                string flag = "VALUE_START";
                vector<string>::iterator judge = find(message.begin(), message.end(), flag);  //���������쳣�ж�
                for (vector<string>::iterator it = message.begin() + 1; it < judge; it++) {
                    InsAttri.push_back(*it);
                }
                //�ж��ֶ��Ƿ�Ϸ�
                if (!tool::eixtAttri(*currentTable, InsAttri)) { //�ֶβ��Ϸ�
                    return false;
                }
                //��ȡ����ֵ
                for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                    value.push_back(*it);
                }
                //��ȡ�����λ��
                tool::GetAttriPosition(*currentTable, InsAttri, &positions);
                //�жϲ����ֵ�����Ե������Ƿ���ͬ
                if (value.size() != InsAttri.size()) {
                    cout << "ֵ���ֶ���Ŀ����ͬ";
                    return false;
                }
                //�жϲ����ֵ�Ƿ����ͷ���
                if (!tool::typeJudge(value, tool::getType(*currentTable, positions))) {

                    return false;
                }
                //todo
                //�ж��Ƿ����Լ������
                if (!tool::judgeConstraints(value, positions, *currentTable)) {
                    return false;
                }
                //����һ��Ԫ��
                int length = currentTable->attributes.size();
                vector<string> tempValue = tool::opInsert(value, positions, length);
                currentTable->values.push_back(tempValue);
                return true;
            }
        }
    }
    //������
    cout << "�����ڱ�" << tableName << endl;
    return false;
}

//ִ�гɹ�����ɾ������������Ż�-1
int
table_opration::Delete(vector<string> message, DB *currentDB)   //message [0]=����  [1]=������=����   ��ʱֻ֧�� and ����������������ȫ�����㣩
{
    //��ȡ����3
    string tableName = message[0];
    int count = 0;  //��¼ɾ��������
    for (int it = 0; it < currentDB->tables.size(); it++) {
        if (tool::compareNoCase(currentDB->tables[it].table_name, tableName)) {
            //��ȡҪ�����ı�
            Table *currentTable = &currentDB->tables[it];
            vector<string> attributeName;
            vector<string> requirementValue;     //���������ֱ�洢������ �� ֵ�� ����һһ��Ӧ
            //�������
            for (int i = 1; i < message.size(); i++) {
                vector<string> temp;
                tool::split(message[i], temp, '='); // ��ʽΪ name=123456 ���� �ָ�Ϊ name  �� 123456
                attributeName.push_back(temp[0]);
                requirementValue.push_back(temp[1]);
            }
            //ȷ��Ҫ�ж����Ե�λ��
            vector<int> position;
            vector<Attribute> currentAttributes = currentTable->attributes;
            //�ж��ֶ��Ƿ�Ϸ�
            if (!tool::eixtAttri(*currentTable, attributeName)) { //�ֶβ��Ϸ�
                return -1;
            }

            tool::GetAttriPosition(*currentTable, attributeName, &position);
            //Ѱ������������Ԫ��
            for (vector<vector<string>>::iterator it = currentTable->values.begin();
                 it < currentTable->values.end(); it++)  //����Ԫ��
            {
                bool flag = true;                           //��־ֵ����flag=true ��ʾ��Ԫ���������������߲�����
                for (int j = 0; j < position.size(); j++)           //�ж��Ƿ�������������
                {
                    vector<string> currentLine = *it;
                    int point = position[j];
                    if (currentLine[point] != requirementValue[j])   //�ж�pointλ���ϵ��ֶε�ֵ���Ƿ��������ֵ
                    {
                        flag = false;   //��Ԫ�鲻��������
                        break;        //ֱ���ж���һ��Ԫ��
                    } //if
                }//for
                if (flag == true) {
                    //ɾ����������������Ԫ��
                    count += 1;
                    currentTable->values.erase(it);
                    it--;  //ɾ��Ҫ����ָ�벻������������ָ��ɾ��Ԫ�ص�ԭ���Ԫ��
                }
            }//for
            return count;
        }
    }
    //������
    cout << "�����ڱ�" << tableName << endl;
    return -1;
}

int table_opration::Update(vector<string> message, DB *currentDB) {
    string tableName = message[0];
    int count = 0; //��¼���µ�����
    for (int it = 0; it < currentDB->tables.size(); it++) {
        if (tool::compareNoCase(currentDB->tables[it].table_name, tableName)) {
            //��ȡҪ�����ı�
            Table *currentTable = &currentDB->tables[it];
            vector<string> attributeName;
            vector<string> requirementValue;     //���������ֱ�洢���µ�����
            vector<string> updateAttribute;
            vector<string> newValue;
            //��ȡ�����ַ���λ�ã������ַ�֮ǰ�Ǹ��µ�ֵ��֮������ж�����
            string flag = "CONDITIONS_START";
            vector<string>::iterator judge = find(message.begin(), message.end(), flag);  //���������쳣�ж�
            vector<string>::iterator start = message.begin();
            //�Ը���ֵ�Ĳ��
            start += 1;  //����ǰ��ı�������ʾ��
            for (vector<string>::iterator it = start; it < judge; it++) {
                vector<string> temp;
                tool::split(*it, temp, '=');
                updateAttribute.push_back(temp[0]);
                newValue.push_back(temp[1]);
            }
            //�������Ĳ��
            for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                vector<string> temp;
                tool::split(*it, temp, '=');  // ��ʽΪ name=123456 ���� �ָ�Ϊ name  �� 123456
                attributeName.push_back(temp[0]);
                requirementValue.push_back(temp[1]);
            }
            //�ж��ֶ��Ƿ�Ϸ�
            if (!tool::eixtAttri(*currentTable, updateAttribute)) { //�����ֶβ��Ϸ�
                return -1;
            }
            if (!tool::eixtAttri(*currentTable, attributeName)) { //�ж��ֶβ��Ϸ�
                return -1;
            }
            //ȷ��������Ҫ�ж����Ե�λ��
            vector<int> position;
            tool::GetAttriPosition(*currentTable, attributeName, &position);
            //ȷ��Ҫ���µ����Ե�λ��
            vector<int> updatePosition;
            tool::GetAttriPosition(*currentTable, updateAttribute, &updatePosition);
            //�ж϶�Ӧλ��Ҫ�����ֵ�Ƿ����ͷ���
            if (!tool::typeJudge(newValue, tool::getType(*currentTable, updatePosition))) {
                return false;
            }
            //Ѱ������������Ԫ�� table position  requirementValue
            for (int i = 0; i < currentTable->values.size(); i++)  //����Ԫ��
            {
                bool flag = true;                           //��־ֵ����flag=true ��ʾ��Ԫ���������������߲�����
                for (int j = 0; j < position.size(); j++)           //�ж��Ƿ�������������
                {
                    vector<string> currentLine = currentTable->values[i];
                    int point = position[j];
                    if (currentLine[point] != requirementValue[j])   //�ж�pointλ���ϵ��ֶε�ֵ���Ƿ��������ֵ
                    {
                        flag = false;   //��Ԫ�鲻��������
                        break;        //ֱ���ж���һ��Ԫ��
                    } //if
                }//for
                if (flag == true) {
                    count += 1;
                    for (int j = 0; j < updatePosition.size(); j++) {
                        vector<string> *currentLine = &currentTable->values[i]; //��ȡ����������Ԫ��
                        int point = updatePosition[j];                    //��ȡ���µ�λ��
                        (*currentLine)[point] = newValue[j];   //����ֵ
                    }//for
                }
            }//for
            return count;
        }
    }
    //������
    cout << "�����ڱ�" << tableName << endl;
    return -1;
}


tablestruct table_opration::Find(vector<string> message, DB currentDB) {
    string tableName = message[0];
    vector<vector<string>> kong;
    for (int it = 0; it < currentDB.tables.size(); it++) {
        if (tool::compareNoCase(currentDB.tables[it].table_name, tableName)) {
//                cout<<"tableName"<<tableName;
            //��ȡҪ�����ı�
            Table currentTable = currentDB.tables[it];
            tablestruct tempValues;  //�洢����������Ԫ��
            vector<string> attributeName;
            vector<string> requirementValue;     //���������ֱ�洢������ �� ֵ�� ����һһ��Ӧ
            vector<string> selectAttri;  //�����洢Ҫ��ʾ���ֶΣ���select id,name from �е�id,name
            string flag = "CONDITIONS_START";//���Ƿָ�ı�־
            vector<string>::iterator judge = find(message.begin(), message.end(), flag); //���ҷָ��־��λ��
            if (message[1] == "*") { //����[1]λ������* �����ѯȫ������
                vector<string> temp; //tempΪ���ú���GetAttriPosition���������������������õ���ʱ����
                temp.push_back("*");
                vector<int> temposition;
                selectAttri = tool::GetAttriPosition(currentTable, temp, &temposition); //����* ����ȡ�����ֶ���
            } else { //���ǻ�ȡ�����ֶ���Ϣ
                //��ȡҪ��ȡ���ֶ���
                for (vector<string>::iterator it = message.begin() + 1;
                     it < judge; it++) //��û��where����������£�judgeָ��ĩβend()
                {
                    selectAttri.push_back(*it);
                }
                //�жϱ����Ǵ����ֶ���
                if (!tool::eixtAttri(currentTable, selectAttri)) { //�ֶβ��Ϸ�
                    return kong;
                }
            }
            tempValues.push_back(selectAttri);   //��Ҫ��ȡ���ֶ�������tempValues,�ᱻ�����ʾ

            if (judge != message.end())//�ҵ��˷ָ�ı�־��˵����where����
            {
                //�������
                for (vector<string>::iterator it = judge + 1; it < message.end(); it++) {
                    vector<string> temp;
                    tool::split(*it, temp, '=');  // ��ʽΪ name=123456 ���� �ָ�Ϊ name  �� 123456
                    attributeName.push_back(temp[0]);
                    requirementValue.push_back(temp[1]);
                }
                //ȷ��Ҫ�ж����Ե�λ��
                vector<int> position;
                vector<string> currentLine;
                //�жϱ����Ǵ����ֶ���
                if (!tool::eixtAttri(currentTable, attributeName)) { //�ֶβ��Ϸ�
                    return kong;
                }
                tool::GetAttriPosition(currentTable, attributeName, &position);
                vector<Attribute> currentAttributes = currentTable.attributes;
                //Ѱ������������Ԫ��
                for (int i = 0; i < currentTable.values.size(); i++)  //����Ԫ��
                {
                    bool flag = true;                           //��־ֵ����flag=true ��ʾ��Ԫ���������������߲�����
                    for (int j = 0; j < position.size(); j++)           //�ж��Ƿ�������������
                    {
                        currentLine = currentTable.values[i];
                        int point = position[j];
                        if (currentLine[point] != requirementValue[j])   //�ж�pointλ���ϵ��ֶε�ֵ���Ƿ��������ֵ
                        {
                            flag = false;   //��Ԫ�鲻��������
                            break;        //ֱ���ж���һ��Ԫ��
                        } //if
                    }//for
                    if (flag == true) {
                        //���������ִ�ж��ֶεĽ�ȡ�������Ժ�ʵ��
                        vector<int> positions;     //��������Էŵ�for ѭ���ⲿ�����߻�һֱ����,��positions��ֵֻ��Ҫ���һ��
                        tool::GetAttriPosition(currentTable, selectAttri, &positions); //��ȡ���Ե�λ���±�
                        tempValues.push_back(tool::GetSelectAttri(currentLine, positions));     //��ȡ����������Ԫ����ض��ֶ�
                    }
                }//for
                if (tempValues.size() == 1) { //��size=1ʱ˵��û������������Ԫ�飬���ؿգ�
                    return kong;
                } else {
                    return tempValues;
                }
            } else { //û�зָ��־��˵��û������,ֱ��������ж�Ӧ�ֶε�������Ϣ
                vector<int> positions;
                tool::GetAttriPosition(currentTable, selectAttri, &positions); //��ȡ���Ե�λ���±�
                for (int i = 0; i < currentTable.values.size(); i++) {
                    vector<string> currentLine = currentTable.values[i];
                    tempValues.push_back(tool::GetSelectAttri(currentLine, positions));
                }
                if (tempValues.size() == 1) { //��size=1ʱ˵��û������������Ԫ�飬���ؿգ�
                    vector<vector<string>> kong;
                    return kong;
                } else {
                    return tempValues;
                }
            }
        }
    }
    //������
    cout << "�����ڱ�" << tableName << endl;
    return kong;
}