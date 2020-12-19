#include <algorithm>
#include <algorithm> //实现find等方法
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

// string.compareNoCase:不区分大小写

bool tool::compareNoCase(std::string strA, const std::string strB) {
    string str1 = strToUpper(strA);
    string str2 = strToUpper(strB);
    return (str1 == str2);
}

//分割字符串，用在tableService.cpp
int tool::split(const string &str, vector<string> &ret_, char sep) {
    if (str.empty()) {
        return 0;
    }

    string tmp;
    string::size_type pos_begin = str.find_first_not_of(
            sep); // 在字符串中查找第一个与str中的字符都不匹配的字符，返回它的位置。搜索从index开始。如果没找到就返回string::nops
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

//获取对应属性的位置放到position中（主要功能）,返回查询的属性名称（主要是当是*时，返回所有属性）
vector<string> tool::GetAttriPosition(Table table, vector<string> attributeName, vector<int> *position) {
    vector<string> reAttri;
    vector<Attribute> currentAttributes;
    vector<Attribute> allAttri = table.attributes;
    if (attributeName[0] == "*") {  //说明查询所有属性
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

//新增
vector<string> tool::opInsert(vector<string> value, vector<int> positions, int length) {
    vector<string> newValue(length, "NULL");//初始化为length长度为NULL的元素
    int position;
    for (int i = 0; i < positions.size(); i++) { //遍历
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
    //获取表的所有字段名
    for (int i = 0; i < allAttri.size(); i++) {
        allAttriname.push_back(allAttri[i].name);
    }
    for (int i = 0; i < attrs.size(); i++) {  //遍历attrs 挨个判断是否存在
        it = find(allAttriname.begin(), allAttriname.end(), attrs[i]);
        if (it != allAttriname.end()) {
            continue;
        } else {
            cout << "不存在字段" << attrs[i] << endl;
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
    vector<string> type;//存储属性类型
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
        regex_search(type, sm, e2)) {//属于能够处理的类型
        return true;
    } else {
        cout << type << "类型不被支持";
        return false;
    }
}

bool tool::typeJudge(vector<string> value, vector<string> type) {
    //匹配char(?) nvarchar(?) int float double
    regex e("^char[(](.*?)[)$]", regex_constants::icase);
    regex e2("^nvarchar[(](.*?)[)$]", regex_constants::icase);
    smatch sm;
    for (int i = 0; i < type.size(); i++) {
        istringstream sin(value[i]);  //先读value入iss
        if (type[i] == "int") {
            int num;
            string::iterator it;
            for (it = value[i].begin(); it < value[i].end(); ++it) {
                if (*it < '0' || *it > '9') {  //判断输入的是数字
                    cout << "第" << i + 1 << "个数据" << value[i] << "的类型为" << type[i] << ",请输入" << type[i] << "类型数据";
                    return false;
                }
            }
            if (!(sin >> num)) {
                cout << "超出" << type[i] << "表示范围";
                return false;
            }
        } else if (type[i] == "float" || type[i] == "double") {
            float num;
            string::iterator it;
            for (it = type[i].begin(); it < type[i].end(); ++it) {
                if ((*it < '0' && *it > '9') && !(*it == '.')) {  //如果不是 数值 和 . 报错
                    cout << "请输入" << type[i] << "类型数据";
                    return false;
                }
            }
            if (!(sin >> num)) {
                cout << "超出" << type[i] << "表示范围";
                return false;
            }
        } else if (regex_search(type[i], sm, e) || regex_search(type[i], sm, e2)) {
            int length = value[i].length();
            int max = stoi(sm[1]);
            if (length > max) {
                cout << "第" << i + 1 << "个数据" << value[i] << "的类型为" << sm[0] << ",长度超长";
                return false;
            }
        }
    }
    return true;
}

void tool::maxLength(vector<vector<string>> values, vector<int> *colMax) {
    int max = 0;
    int tolCol = values[0].size(); //获取总的列数
    int tolRow = values.size();   //总列数
    int temp;
    for (int col = 0; col < tolCol; col++) {  //列数不变，遍历行
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
    vector<vector<string>> AttriMessage; //存储返回值，即格式化后的属性信息
    vector<Attribute> currentAttris; //存储未格式化的字段信息
    Attribute attri;
    string first[] = {"Field", "Type", "Null", "Key", "Unique"};
    vector<string> temp;
    for (int i = 0; i < first->length(); i++) {  //把头标志入容器
        temp.push_back(first[i]);
    }
    AttriMessage.push_back(temp);
    for (int i = 0; i < currentDB.tables.size(); i++) {    //在DB中查找对应的表
        if (tool::compareNoCase(currentDB.tables[i].table_name, tableName)) {
            currentAttris = currentDB.tables[i].attributes;
            for (int i = 0; i < currentAttris.size(); i++) {   // 遍历说有属性，并格式化
                temp.clear();   //清空temp
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
    vector<string> colMessage; //存储一列的信息
    for (int row = 0; row < rows; row++) {
        colMessage.push_back(values[row][i]);
    }
    return colMessage;
}

bool tool::judgeConstraints(vector<string> value, vector<int> positions, Table table) {
    vector<Attribute> attris = table.attributes;
    vector<vector<string>> values = table.values;
    vector<string> colValue; //存储一列的所有信息
    int point; //表示要判断的属性的位置
    for (int i = 0; i < positions.size(); i++) {
        point = positions[i];
        int flag = 0; //判断满足的个数
        // cout<<value[i]<<"  "<<point<<"  "<<attris[point].name<<endl;
        if (attris[point].is_Not_Null == true) {
            //判断是否为空值NULL
            if (value[i] == "NULL") {
                  cout<<"值"<<value[i]<<"不满足非空约束";
                return false;
            }
        }
        if (attris[point].is_Unique == true) {
            //判断值是否重复
            //1.获取该列的所有值
            colValue = tool::getCol(values, point);
            //find 查找是否存在重复
            vector<string>::iterator it = find(colValue.begin(), colValue.end(), value[i]);
            if (it != colValue.end()) {
                    cout<<"值"<<value[i]<<"不满足Unique约束";
                return false;
            }
        }
        if (attris[point].is_Key == true) { //没有return false 说明满足前面两个条件，返回true
            return true;
        }
        return true;
    }
}
/*  备用
vector<string> Interpreter::split(const string &s, const string &seperator) {
    vector<string> result;
    typedef string::size_type string_size;
    string_size i = 0;

    while (i != s.size()) {
        //找到字符串中首个不等于分隔符的字母；
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

        //找到又一个分隔符，将两个分隔符之间的字符串取出；
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
        //提取表名 tbName
        int id = sql.find("TABLE");
        int id2 =sql.find("(");
        string subs = sql.substr(id+6, id2-(id+6));
        params.push_back(subs);
        //提取（）中的部分
        int id3 = sql.find(");");
        sql = sql.substr(id2+1, id3-(id2+1));
        vector<string> allParams;
        tool::split(sql,allParams,',');
        for(vector<string>::size_type i=0; i<allParams.size(); i++){
            string tmpStr = allParams[i];
            vector<string> tmpParams;
            tool::split(tmpStr,tmpParams,' ');
            //字段名
            params.push_back(tmpParams[0]);
            //字段类型
            params.push_back(tmpParams[1]);
            //约束类型
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

        // 取出')'和'('和 ','在字符串中的下标，方便后面提取参数
        vector<int> ids;
        for (string::size_type i = 0; i < sql.size(); i++) {
            if (sql.at(i) == '(' || sql.at(i) == ')' || sql.at(i) == ',') {
                ids.push_back(i);
            }
        }
        // 提取参数
        for (int i = 1; i < ids.size(); i++) {
            subs = sql.substr(ids[i - 1] + 1, ids[i] - (ids[i - 1] + 1));
            params.push_back(subs);
        }
    }*/

