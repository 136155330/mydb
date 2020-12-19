
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include "interpreter.h"
#include "tool.h"  //自定义工具类 忽略大小写比较string

using std::string;
using std::vector;
using std::cin;
using std::cout;
using std::endl;
using std::regex;
using namespace std;

vector<string> Interpreter::analyse(string sql) {
    vector<string> params;
    regex pattern;

    /// 权限操作语句
    // GRANT
    pattern = "^GRANT{1}.{1,}ON{1}.{1,}TO{1}.{1,}+";
    if (regex_match(sql, pattern)) {
        int id = sql.find("GRANT");
        int id2 = sql.find("ON");
        int id3 = sql.find("TO");
        int id4 = sql.find(";") + 1;

        string subs = "GRANT";
        params.push_back(subs);
        subs = sql.substr(id + 6, id2 - (id + 6) - 1);
        params.push_back(subs);
        subs = sql.substr(id2 + 3, id3 - (id2 + 3) - 1);
        params.push_back(subs);
        subs = sql.substr(id3 + 3, id4 - (id3 + 3) - 1);
        params.push_back(subs);
    }

    //REVOKE
    pattern = "^REVOKE{1}.{1,}ON{1}.{1,}TO{1}.{1,}+";
    if (regex_match(sql, pattern)) {
        int id = sql.find("REVOKE");
        int id2 = sql.find("ON");
        int id3 = sql.find("TO");
        int id4 = sql.find(";") + 1;

        string subs = "REVOKE";
        params.push_back(subs);
        subs = sql.substr(id + 7, id2 - (id + 7) - 1);
        params.push_back(subs);
        subs = sql.substr(id2 + 3, id3 - (id2 + 3) - 1);
        params.push_back(subs);
        subs = sql.substr(id3 + 3, id4 - (id3 + 3) - 1);
        params.push_back(subs);
    }

    ///库操作语句
    // CREATE DB
    regex patternDB("^CREATE{1} {1,}DATABASE{1}.{1,};", regex_constants::icase);
    if (regex_match(sql, patternDB)) {
        string subs = "CREATE_DB";
        params.push_back(subs);
        transform(sql.begin(), sql.end(), sql.begin(), towupper);
        int id1 = sql.find("DATABASE");
        int id2 = sql.find(";") + 1;
        subs = sql.substr(id1 + 9, id2 - (id1 + 9) - 1);
        params.push_back(subs);
    }

    // DROP DB
    regex patternDropDB("^DROP{1} {1,}DATABASE{1}.{1,};", regex_constants::icase);
    if (regex_match(sql, patternDropDB)) {
        string subs = "DROP_DB";
        params.push_back(subs);
        transform(sql.begin(), sql.end(), sql.begin(), towupper);
        int id1 = sql.find("DATABASE");
        int id2 = sql.find(";") + 1;
        subs = sql.substr(id1 + 9, id2 - (id1 + 9) - 1);
        params.push_back(subs);
    }

    //SHOW DB
    regex patternShowDB("^SHOW{1} {1,}DATABASES;+", regex_constants::icase);
    if (regex_match(sql, patternShowDB)) {
        string subs = "SHOW_DB";
        params.push_back(subs);
    }

    //USE DB
    regex patternUDB("^USE{1} {1,}.*;", regex_constants::icase);
    if (regex_match(sql, patternUDB)) {
        string subs = "USE_DB";
        params.push_back(subs);
        transform(sql.begin(), sql.end(), sql.begin(), towupper);
        int id1 = sql.find("USE");
        int id2 = sql.find(";") + 1;
        subs = sql.substr(id1 + 4, id2 - (id1 + 4) - 1);
        params.push_back(subs);
    }

    /// 表操作语句
    //CREATE_TABLE
    //string s ="CREATE TABLE student(name int,id char(10) primary key,hobby nvarchar(10),teatche int  );";
    regex e("CREATE TABLE (\\w+)[ ]{0,}\\((.*?)[ ]{0,}\\);$",
            regex_constants::icase); //忽略大小写， \\){0,} 的意思是防止遇到 char(10 )就结束匹配了，这里会用贪婪匹配，尽可能多的匹配）
    smatch sm;    // same as std::match_results<const char*> cm;
    if (regex_search(sql, sm, e)) {
        string temp;
        smatch sm2;
        smatch sm3;
        regex e2("(\\w+ .*?),"); //第一个括号匹配每空格字符 到空格结束，第二个一样，空格? 表示有空格或没有空格判断有没有约束存在
        regex e3("([^ ]+) ([^ ]+) ?(.+){0,}"); //拆分属性，类型，和约束
        temp = sm[2];//括号中间的所有内容
        string subs = "CREATE_TABLE";
        params.push_back(subs);
        params.push_back(sm[1]);//存入表名
        while (regex_search(temp, sm2, e2)) {
            string temp2 = sm2[1]; //sm2[0]带有逗号，不能要
            if (regex_search(temp2, sm3, e3)) {
                for (int m = 1; m < sm3.size(); m++) {  //[0]是匹配的整个字符串
                    string get = sm3[m];
                    params.push_back(sm3[m]);
                }
            }
            temp = sm2.suffix().str();//查找是否还有匹配的字符串
        }
        //此时，temp中保存的是sm2.suffix（）的值
        if (regex_search(temp, sm3, e3)) { //最后一个匹配不到，因为没有逗号结尾，但是保存在sm2.suffix中
            for (int m = 1; m < sm3.size(); m++) {  //[0]是匹配的整个字符串
                params.push_back(sm3[m]);
            }
        }
    }

    //DROP_TB
    pattern = "^DROP{1} {1,}TABLE .{1,};+";
    if (regex_match(sql, pattern)) {
        int id = sql.find("TABLE");
        int id2 = sql.find(";") + 1;

        string subs = "DROP_TB";
        params.push_back(subs);
        subs = sql.substr(id + 6, id2 - (id + 6) - 1);
        params.push_back(subs);
    }


    //INSERT_TABLE
    //string s = "INSERT INTO class VALUES(wudao,002,run);";
    //      （\\w)匹配[a-zA-Z_0-9] (.*?)非贪婪匹配，尽可能匹配少的
    //       [(]?表示可能有        [ ]{0,}表示空格可有可无
    regex e2("insert into (\\w+) *[(]{0,1}(.*?)?[)]?[ ]{1,}values *\\((.*?)\\);$", regex_constants::icase);
    //匹配结果  整体 + 表名+属性1，属性2+值1，值二
    if (regex_search(sql, sm, e2)) {
        string subs = "INSERT_TABLE";
        params.push_back(subs);
        params.push_back(sm[1]);//存入表名
        string temp;
        temp = sm[2];
        int i = 2;
        int flag = true; //表示不是一次插入全部属性
        if (temp == "") {
            i++;
            params.push_back(temp);//存入空值，代表全部属性
            params.push_back("VALUE_START");
            flag = false;
        }
        regex e("(\\w+),?"); //第二次匹配，拆分属性和拆分要插入的值 属性1，属性2，属性3
        for (i; i < sm.size(); i++) {  //整句话被分为4段，这里会循环一次或两次
            smatch sm2;
            temp = sm[i];
            while (regex_search(temp, sm2, e)) {
                params.push_back(sm2[1]);  //[0]中的数据有逗号
                temp = sm2.suffix();//获取还没有匹配到的部分，再次匹配
            }
            if (flag) { //如果true,说明不是插入全部属性，插入标志位
                params.push_back("VALUE_START");
                flag = false;
            }
        }

    }
    // DELETE
    regex patternDelete("^DELETE{1} {1,}FROM.{1,}WHERE.{1,}+;$", regex_constants::icase);
    if (regex_match(sql, patternDelete)) {
        params.push_back("DELETE");
        vector<string> allParams;
        tool::split(sql, allParams, ' ');
        for (vector<string>::size_type i = 0; i < allParams.size(); i++) {
            string tmpStr = allParams[i];
            if (tool::compareNoCase(tmpStr, "DELETE") || tool::compareNoCase(tmpStr, "FROM") ||
                tool::compareNoCase(tmpStr, "WHERE") || tool::compareNoCase(tmpStr, "AND"))
                continue;
            params.push_back(tmpStr);
        }
        //params中最后一个String 是 属性1=value,属性2=value;属性3=value；
        string str = params[params.size() - 1];  //取出要拆分的元素
        params.pop_back(); //删除最后一个元素
        regex e("(\\w+[ ]{0,}= {0,}\\w+),?;?");  //拆分格式 属性1=value,属性2=value;属性3=value； 匹配结果 name=lisi    id=123
        smatch sm;
        while (regex_search(str, sm, e)) {
            params.push_back(sm[1]);  //sm[0]中是匹配到的整个字符串
            str = sm.suffix();//获取还没有匹配到的部分，再次匹配
        }
    }

    // UPDATE
    regex patternUpdate("^UPDATE{1}.{1,}SET{1}.{1,}WHERE.{1,}+;$", regex_constants::icase);
    if (regex_match(sql, patternUpdate)) {
        params.push_back("UPDATE");
        vector<string> allParams;
        tool::split(sql, allParams, ' ');
        regex e("(\\w+[ ]{0,}= {0,}\\w+),?"); //第二次匹配，拆分属性和拆分要插入的值 属性1，属性2，属性3
        smatch sm2;
        for (vector<string>::size_type i = 0; i < allParams.size(); i++) {
            string tmpStr = allParams[i];
            if (tool::compareNoCase(tmpStr, "UPDATE") || tool::compareNoCase(tmpStr, "AND"))
                continue;
            if (tool::compareNoCase(tmpStr, "SET")) {
                continue;
            } else if (tool::compareNoCase(tmpStr, "WHERE")) {
                params.push_back("CONDITIONS_START");
            } else {
                if (tmpStr[tmpStr.size() - 1] == ',' || tmpStr[tmpStr.size() - 1] == ';')
                    tmpStr.erase(tmpStr.size() - 1);  //tempStr 格式name=wudao,id=001
                bool flag = regex_search(tmpStr, sm2, e); //考虑第一个是表名，无法匹配，但是要存入params
                if (!flag) { params.push_back(tmpStr); }      //存入表名
                while (flag) {
                    params.push_back(sm2[1]);  //[0]中的数据有逗号
                    tmpStr = sm2.suffix();//获取还没有匹配到的部分，再次匹配
                    flag = regex_search(tmpStr, sm2, e);
                }
            }
        }
    }

    // 查询 SELECT
    regex patternSelect("^SELECT{1}.{1,}FROM {1}.{1,}+;$", regex_constants::icase);
    if (regex_match(sql, patternSelect)) {
        params.push_back("SELECT");
        vector<string> allParams;
        tool::split(sql, allParams, ' ');
        //找到表名 tbName
        for (vector<string>::size_type i = 0; i < allParams.size() - 1; i++) {
            if (tool::compareNoCase(allParams[i], "FROM")) {
                string str = allParams[i + 1];

                string::iterator it;              //指向string类的迭代器。你可以理解为指针
                for (it = str.begin(); it <= str.end(); it++) {
                    if (*it == ';')
                        str.erase(it);          //删除it处的一个字符
                }
                params.push_back(tool::trim(str));
            }
        }

        for (vector<string>::size_type i = 0; i < allParams.size(); i++) {
            string tmpStr = allParams[i];
            if (tool::compareNoCase(tmpStr, "FROM") || tool::compareNoCase(tmpStr, "AND") ||
                tool::compareNoCase(tmpStr, params[1]))
                continue;
            if (tool::compareNoCase(tmpStr, "SELECT")) {
                continue;
//                params.push_back("FIELD_START");
            } else if (tool::compareNoCase(tmpStr, "WHERE")) {
                params.push_back("CONDITIONS_START");
            } else {
                if (tmpStr[tmpStr.size() - 1] == ',' || tmpStr[tmpStr.size() - 1] == ';')
                    tmpStr.erase(tmpStr.size() - 1);
                params.push_back(tmpStr);
            }
        }
        //增加begin
        if (params[2] != "*") {
            regex e("(\\w+),?"); //第二次匹配，拆分属性和拆分要插入的值 属性1，属性2，属性3
            smatch sm2;
            string temp = params[2]; //获取格式如下，name,id,hobby
            int i = 2;
            params.erase(params.begin() + i);
            while (regex_search(temp, sm2, e)) {
                params.insert(params.begin() + i, sm2[1]);  //[0]中的数据有逗号
                temp = sm2.suffix();//获取还没有匹配到的部分，再次匹配
                i++;
            }
        }

        //增加end
        string str = params[params.size() - 1];  //取出要拆分的元素
        params.pop_back(); //删除最后一个元素
        regex e2("(\\w+[ ]{0,}= {0,}\\w+),?;?");  //拆分格式 属性1=value,属性2=value;属性3=value； 匹配结果 name=lisi    id=123
        smatch sm;
        while (regex_search(str, sm, e2)) {
            params.push_back(sm[1]);  //sm[0]中是匹配到的整个字符串
            str = sm.suffix();//获取还没有匹配到的部分，再次匹配
        }
    }


    regex patternShowTB("^SHOW{1} {1,}TABLES;", regex_constants::icase);
    if (regex_match(sql, patternShowTB)) {
        string subs = "SHOW_TB";
        params.push_back(subs);
    }


    ///CREATE VIEW
    regex patternView("^CREATE VIEW .{1,} AS SELECT{1}.{1,}FROM {1}.{1,}+", regex_constants::icase);
    if (regex_match(sql, patternView)) {
        string subs = "CREATE_VIEW";
        params.push_back(subs);

        int id = sql.find("VIEW");
        int id2 = sql.find("AS");
        subs = sql.substr(id + 5, (id2 - 1) - (id + 5));
        params.push_back(subs);

        int id3 = sql.find("SELECT");
        string tmp = sql.substr(id3, sql.size() - id3);

        Interpreter interpreter;
        vector<string> vecSelect = interpreter.analyse(tmp);

        if (vecSelect.size() == 0) {
            params.clear();
        } else {
            for (int i = 1; i < vecSelect.size(); i++) {
                params.push_back(vecSelect[i]);
            }

        }

    }

    ///DROP VIEW
    pattern = "^DROP VIEW .{1,};+";
    if (regex_match(sql, pattern)) {
        int id = sql.find("VIEW");
        int id2 = sql.find(";") + 1;

        string subs = "DROP_VIEW";
        params.push_back(subs);
        subs = sql.substr(id + 5, id2 - (id + 5) - 1);
        params.push_back(subs);
    }


    ///COMMIT;

    regex patternCommit("^COMMIT;", regex_constants::icase);
    if (regex_match(sql, patternCommit)) {
        string subs = "COMMIT";
        params.push_back(subs);
    }

    ///EXIT;
    regex eExit("^exit;$", regex_constants::icase); //忽略大小写
    if (regex_match(sql, sm, eExit)) {
        string subs = "EXIT";
        params.push_back(subs);
    }

    ///describe table
    regex eDesc(" *describe +(\\w+);$", regex_constants::icase);
    if (regex_match(sql, sm, eDesc)) {
        string subs = "DESC_TABLE";
        string tbName = sm[1];
        params.push_back(subs);
        params.push_back(tbName);

    }
    return params;
}


