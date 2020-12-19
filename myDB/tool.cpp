#include <algorithm>
#include <algorithm> //ʵ��find�ȷ���
#include <string>
#include <regex>
#include "tool.h"
#include "DB.h"

using namespace std;

string tool::strToUpper(string str) {
    std::string strTmp = str;
    transform(strTmp.begin(), strTmp.end(), strTmp.begin(), towupper);
    return strTmp;
}

// string.compareNoCase:�����ִ�Сд

bool tool::compareNoCase(std::string strA, const std::string strB) {
    string str1 = strToUpper(strA);
    string str2 = strToUpper(strB);
    return (str1 == str2);
}

//�ָ��ַ���������tableService.cpp
int tool::split(const string &str, vector<string> &ret_, char sep) {
    if (str.empty()) {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(
            sep); // ���ַ����в��ҵ�һ����str�е��ַ�����ƥ����ַ�����������λ�á�������index��ʼ�����û�ҵ��ͷ���string::nops
    string::size_type comma_pos = 0;

    while (pos_begin != string::npos) {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != string::npos) {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + 1;
        } else {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty()) {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

//��ȡ��Ӧ���Ե�λ�÷ŵ�position�У���Ҫ���ܣ�,���ز�ѯ���������ƣ���Ҫ�ǵ���*ʱ�������������ԣ�
vector<string> tool::GetAttriPosition(Table table, vector<string> attributeName, vector<int> *position) {
    vector<string> reAttri;
    vector<Attribute> currentAttributes;
    vector<Attribute> allAttri = table.attributes;
    if (attributeName[0] == "*") {  //˵����ѯ��������
        for (int i = 0; i < allAttri.size(); i++) {
            reAttri.push_back(allAttri[i].name);
            (*position).push_back(i);
        }
        return reAttri;
    } else {
        for (int i = 0; i < attributeName.size(); i++) {
            for (int j = 0; j < allAttri.size(); j++) {
                if (tool::compareNoCase(attributeName[i], allAttri[j].name)) {
                    position->push_back(j);
                    break;
                }
            }
        }
        return attributeName;
    }

}

vector<string> tool::GetSelectAttri(vector<string> value, vector<int> positions) {
    vector<string> getValue;
    int position;
    for (int i = 0; i < positions.size(); i++) {
        position = positions[i];
        getValue.push_back(*(value.begin() + position));
    }
    return getValue;
}

//����
vector<string> tool::opInsert(vector<string> value, vector<int> positions, int length) {
    vector<string> newValue(length, "NULL");//��ʼ��Ϊlength����ΪNULL��Ԫ��
    int position;
    for (int i = 0; i < positions.size(); i++) { //����
        position = positions[i];
        newValue[position] = value[i];
    }
    return newValue;
}


string tool::trim(string str) {
    if (str.empty()) { return str; }
    str.erase(0, str.find_first_not_of(" "));
    str.erase(str.find_last_not_of(" ") + 1);
    return str;
}


bool tool::eixtAttri(Table table, vector<string> attrs) {
    vector<string>::iterator it;
    vector<Attribute> allAttri = table.attributes;
    vector<string> allAttriname;
    //��ȡ��������ֶ���
    for (int i = 0; i < allAttri.size(); i++) {
        allAttriname.push_back(allAttri[i].name);
    }
    for (int i = 0; i < attrs.size(); i++) {  //����attrs �����ж��Ƿ����
        it = find(allAttriname.begin(), allAttriname.end(), attrs[i]);
        if (it != allAttriname.end()) {
            continue;
        } else {
            cout << "�������ֶ�" << attrs[i] << endl;
            return false;
        }
    }
    return true;
}

bool tool::eixtTable(DB currentDB, string tablename) {
    vector<string> allTableName = currentDB.showTables();
    vector<string>::iterator it = find(allTableName.begin(), allTableName.end(), tablename);
    if (it == allTableName.end())
        return false;
    return true;
}


vector<string> tool::getType(Table table, vector<int> positions) {
    vector<string> type;//�洢��������
    vector<Attribute> allAttri;
    allAttri = table.attributes;
    int position;
    for (int i = 0; i < positions.size(); i++) {
        position = positions[i];
        type.push_back(allAttri[position].type);
    }
    return type;
}

bool tool::typeExit(string type) {
    regex e("^char[(](.*?)[)$]", regex_constants::icase);
    regex e2("^nvarchar[(](.*?)[)$]", regex_constants::icase);
    smatch sm;
    if (type == "int" || type == "float" || type == "double" || regex_search(type, sm, e) ||
        regex_search(type, sm, e2)) {//�����ܹ����������
        return true;
    } else {
        cout << type << "���Ͳ���֧��";
        return false;
    }
}

bool tool::typeJudge(vector<string> value, vector<string> type) {
    //ƥ��char(?) nvarchar(?) int float double
    regex e("^char[(](.*?)[)$]", regex_constants::icase);
    regex e2("^nvarchar[(](.*?)[)$]", regex_constants::icase);
    smatch sm;
    for (int i = 0; i < type.size(); i++) {
        istringstream sin(value[i]);  //�ȶ�value��iss
        if (type[i] == "int") {
            int num;
            string::iterator it;
            for (it = value[i].begin(); it < value[i].end(); ++it) {
                if (*it < '0' || *it > '9') {  //�ж������������
                    cout << "��" << i + 1 << "������" << value[i] << "������Ϊ" << type[i] << ",������" << type[i] << "��������";
                    return false;
                }
            }
            if (!(sin >> num)) {
                cout << "����" << type[i] << "��ʾ��Χ";
                return false;
            }
        } else if (type[i] == "float" || type[i] == "double") {
            float num;
            string::iterator it;
            for (it = type[i].begin(); it < type[i].end(); ++it) {
                if ((*it < '0' && *it > '9') && !(*it == '.')) {  //������� ��ֵ �� . ����
                    cout << "������" << type[i] << "��������";
                    return false;
                }
            }
            if (!(sin >> num)) {
                cout << "����" << type[i] << "��ʾ��Χ";
                return false;
            }
        } else if (regex_search(type[i], sm, e) || regex_search(type[i], sm, e2)) {
            int length = value[i].length();
            int max = stoi(sm[1]);
            if (length > max) {
                cout << "��" << i + 1 << "������" << value[i] << "������Ϊ" << sm[0] << ",���ȳ���";
                return false;
            }
        }
    }
    return true;
}

void tool::maxLength(vector<vector<string>> values, vector<int> *colMax) {
    int max = 0;
    int tolCol = values[0].size(); //��ȡ�ܵ�����
    int tolRow = values.size();   //������
    int temp;
    for (int col = 0; col < tolCol; col++) {  //�������䣬������
        for (int row = 0; row < tolRow; row++) {
            temp = values[row][col].length();
            if (temp > max) {
                max = temp;
            }
        }
        (*colMax).push_back(max);
        max = 0;
    }
}

vector<vector<string>> tool::descTable(DB currentDB, string tableName) {
    vector<vector<string>> AttriMessage; //�洢����ֵ������ʽ�����������Ϣ
    vector<Attribute> currentAttris; //�洢δ��ʽ�����ֶ���Ϣ
    Attribute attri;
    string first[] = {"Field", "Type", "Null", "Key", "Unique"};
    vector<string> temp;
    for (int i = 0; i < first->length(); i++) {  //��ͷ��־������
        temp.push_back(first[i]);
    }
    AttriMessage.push_back(temp);
    for (int i = 0; i < currentDB.tables.size(); i++) {    //��DB�в��Ҷ�Ӧ�ı�
        if (tool::compareNoCase(currentDB.tables[i].table_name, tableName)) {
            currentAttris = currentDB.tables[i].attributes;
            for (int i = 0; i < currentAttris.size(); i++) {   // ����˵�����ԣ�����ʽ��
                temp.clear();   //���temp
                attri = currentAttris[i];
                temp.push_back(attri.name);
                temp.push_back(attri.type);
                if (attri.is_Not_Null == true)
                    temp.push_back("NO");
                else
                    temp.push_back("YES");
                if (attri.is_Key == true)
                    temp.push_back("YES");
                else
                    temp.push_back("");
                if (attri.is_Unique == true)
                    temp.push_back("YES");
                else
                    temp.push_back("NO");
                AttriMessage.push_back(temp);
            }

        }
    }
    return AttriMessage;
}

vector<string> tool::getCol(vector<vector<string>> values, int i) {
    int rows = values.size();
    vector<string> colMessage; //�洢һ�е���Ϣ
    for (int row = 0; row < rows; row++) {
        colMessage.push_back(values[row][i]);
    }
    return colMessage;
}

bool tool::judgeConstraints(vector<string> value, vector<int> positions, Table table) {
    vector<Attribute> attris = table.attributes;
    vector<vector<string>> values = table.values;
    vector<string> colValue; //�洢һ�е�������Ϣ
    int point; //��ʾҪ�жϵ����Ե�λ��
    for (int i = 0; i < positions.size(); i++) {
        point = positions[i];
        int flag = 0; //�ж�����ĸ���
        // cout<<value[i]<<"  "<<point<<"  "<<attris[point].name<<endl;
        if (attris[point].is_Not_Null == true) {
            //�ж��Ƿ�Ϊ��ֵNULL
            if (value[i] == "NULL") {
                  cout<<"ֵ"<<value[i]<<"������ǿ�Լ��";
                return false;
            }
        }
        if (attris[point].is_Unique == true) {
            //�ж�ֵ�Ƿ��ظ�
            //1.��ȡ���е�����ֵ
            colValue = tool::getCol(values, point);
            //find �����Ƿ�����ظ�
            vector<string>::iterator it = find(colValue.begin(), colValue.end(), value[i]);
            if (it != colValue.end()) {
                    cout<<"ֵ"<<value[i]<<"������UniqueԼ��";
                return false;
            }
        }
        if (attris[point].is_Key == true) { //û��return false ˵������ǰ����������������true
            return true;
        }
        return true;
    }
}
/*  ����
vector<string> Interpreter::split(const string &s, const string &seperator) {
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //�ҵ��ַ������׸������ڷָ�������ĸ��
        int flag = 0;
        while (i != s.size() && flag == 0) {
            flag = 1;
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[i] == seperator[x]) {
                    ++i;
                    flag = 0;
                    break;

                }
        }

        //�ҵ���һ���ָ������������ָ���֮����ַ���ȡ����
        flag = 0;
        string_size j = i;
        while (j != s.size() && flag == 0) {
            for (string_size x = 0; x < seperator.size(); ++x)
                if (s[j] == seperator[x]) {
                    flag = 1;
                    break;

                }
            if (flag == 0)
                ++j;
        }
        if (i != j) {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}*/

/*   //CREATE_TB
    pattern = "^CREATE{1}.{1,}TABLE{1} {1,}.{1,} {1,}(.*)";
    if (regex_match(sql, pattern)) {
        params.push_back("CREATE_TB");
        //��ȡ���� tbName
        int id = sql.find("TABLE");
        int id2 =sql.find("(");
        string subs = sql.substr(id+6, id2-(id+6));
        params.push_back(subs);
        //��ȡ�����еĲ���
        int id3 = sql.find(");");
        sql = sql.substr(id2+1, id3-(id2+1));
        vector<string> allParams;
        tool::split(sql,allParams,',');
        for(vector<string>::size_type i=0; i<allParams.size(); i++){
            string tmpStr = allParams[i];
            vector<string> tmpParams;
            tool::split(tmpStr,tmpParams,' ');
            //�ֶ���
            params.push_back(tmpParams[0]);
            //�ֶ�����
            params.push_back(tmpParams[1]);
            //Լ������
            if(tmpStr.find("PRIMARY KEY")!=string::npos){
                params.push_back("PRIMARY_KEY");
            }else if(tmpStr.find("NOT NULL")!=string::npos){
                params.push_back("NOT_NULL");
            }else if(tmpStr.find("UNIQUE")!=string::npos){
                params.push_back("UNIQUE");
            }else{
                params.push_back("EMPTY");
            }
        }
    }*/

/*    // INSERT
    pattern = "^INSERT{1} {1,}INTO .{1,}VALUES(.{0,})+";
    if (regex_match(sql, pattern)) {
        int id = sql.find("INTO");
        int id2 = sql.find("VALUES") - 1;

        string subs = "INSERT";
        params.push_back(subs);
        subs = sql.substr(id + 5, id2 - (id + 5));
        params.push_back(subs);

        // ȡ��')'��'('�� ','���ַ����е��±꣬���������ȡ����
        vector<int> ids;
        for (string::size_type i = 0; i < sql.size(); i++) {
            if (sql.at(i) == '(' || sql.at(i) == ')' || sql.at(i) == ',') {
                ids.push_back(i);
            }
        }
        // ��ȡ����
        for (int i = 1; i < ids.size(); i++) {
            subs = sql.substr(ids[i - 1] + 1, ids[i] - (ids[i - 1] + 1));
            params.push_back(subs);
        }
    }*/

