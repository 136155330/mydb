
#include <vector>
#include <string>
#include <iostream>
#include <regex>
#include "interpreter.h"
#include "tool.h"  //�Զ��幤���� ���Դ�Сд�Ƚ�string

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

    /// Ȩ�޲������
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

    ///��������
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

    /// ��������
    //CREATE_TABLE
    //string s ="CREATE TABLE student(name int,id char(10) primary key,hobby nvarchar(10),teatche int  );";
    regex e("CREATE TABLE (\\w+)[ ]{0,}\\((.*?)[ ]{0,}\\);$",
            regex_constants::icase); //���Դ�Сд�� \\){0,} ����˼�Ƿ�ֹ���� char(10 )�ͽ���ƥ���ˣ��������̰��ƥ�䣬�����ܶ��ƥ�䣩
    smatch sm;    // same as std::match_results<const char*> cm;
    if (regex_search(sql, sm, e)) {
        string temp;
        smatch sm2;
        smatch sm3;
        regex e2("(\\w+ .*?),"); //��һ������ƥ��ÿ�ո��ַ� ���ո�������ڶ���һ�����ո�? ��ʾ�пո��û�пո��ж���û��Լ������
        regex e3("([^ ]+) ([^ ]+) ?(.+){0,}"); //������ԣ����ͣ���Լ��
        temp = sm[2];//�����м����������
        string subs = "CREATE_TABLE";
        params.push_back(subs);
        params.push_back(sm[1]);//�������
        while (regex_search(temp, sm2, e2)) {
            string temp2 = sm2[1]; //sm2[0]���ж��ţ�����Ҫ
            if (regex_search(temp2, sm3, e3)) {
                for (int m = 1; m < sm3.size(); m++) {  //[0]��ƥ��������ַ���
                    string get = sm3[m];
                    params.push_back(sm3[m]);
                }
            }
            temp = sm2.suffix().str();//�����Ƿ���ƥ����ַ���
        }
        //��ʱ��temp�б������sm2.suffix������ֵ
        if (regex_search(temp, sm3, e3)) { //���һ��ƥ�䲻������Ϊû�ж��Ž�β�����Ǳ�����sm2.suffix��
            for (int m = 1; m < sm3.size(); m++) {  //[0]��ƥ��������ַ���
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
    //      ��\\w)ƥ��[a-zA-Z_0-9] (.*?)��̰��ƥ�䣬������ƥ���ٵ�
    //       [(]?��ʾ������        [ ]{0,}��ʾ�ո���п���
    regex e2("insert into (\\w+) *[(]{0,1}(.*?)?[)]?[ ]{1,}values *\\((.*?)\\);$", regex_constants::icase);
    //ƥ����  ���� + ����+����1������2+ֵ1��ֵ��
    if (regex_search(sql, sm, e2)) {
        string subs = "INSERT_TABLE";
        params.push_back(subs);
        params.push_back(sm[1]);//�������
        string temp;
        temp = sm[2];
        int i = 2;
        int flag = true; //��ʾ����һ�β���ȫ������
        if (temp == "") {
            i++;
            params.push_back(temp);//�����ֵ������ȫ������
            params.push_back("VALUE_START");
            flag = false;
        }
        regex e("(\\w+),?"); //�ڶ���ƥ�䣬������ԺͲ��Ҫ�����ֵ ����1������2������3
        for (i; i < sm.size(); i++) {  //���仰����Ϊ4�Σ������ѭ��һ�λ�����
            smatch sm2;
            temp = sm[i];
            while (regex_search(temp, sm2, e)) {
                params.push_back(sm2[1]);  //[0]�е������ж���
                temp = sm2.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
            }
            if (flag) { //���true,˵�����ǲ���ȫ�����ԣ������־λ
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
        //params�����һ��String �� ����1=value,����2=value;����3=value��
        string str = params[params.size() - 1];  //ȡ��Ҫ��ֵ�Ԫ��
        params.pop_back(); //ɾ�����һ��Ԫ��
        regex e("(\\w+[ ]{0,}= {0,}\\w+),?;?");  //��ָ�ʽ ����1=value,����2=value;����3=value�� ƥ���� name=lisi    id=123
        smatch sm;
        while (regex_search(str, sm, e)) {
            params.push_back(sm[1]);  //sm[0]����ƥ�䵽�������ַ���
            str = sm.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
        }
    }

    // UPDATE
    regex patternUpdate("^UPDATE{1}.{1,}SET{1}.{1,}WHERE.{1,}+;$", regex_constants::icase);
    if (regex_match(sql, patternUpdate)) {
        params.push_back("UPDATE");
        vector<string> allParams;
        tool::split(sql, allParams, ' ');
        regex e("(\\w+[ ]{0,}= {0,}\\w+),?"); //�ڶ���ƥ�䣬������ԺͲ��Ҫ�����ֵ ����1������2������3
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
                    tmpStr.erase(tmpStr.size() - 1);  //tempStr ��ʽname=wudao,id=001
                bool flag = regex_search(tmpStr, sm2, e); //���ǵ�һ���Ǳ������޷�ƥ�䣬����Ҫ����params
                if (!flag) { params.push_back(tmpStr); }      //�������
                while (flag) {
                    params.push_back(sm2[1]);  //[0]�е������ж���
                    tmpStr = sm2.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
                    flag = regex_search(tmpStr, sm2, e);
                }
            }
        }
    }

    // ��ѯ SELECT
    regex patternSelect("^SELECT{1}.{1,}FROM {1}.{1,}+;$", regex_constants::icase);
    if (regex_match(sql, patternSelect)) {
        params.push_back("SELECT");
        vector<string> allParams;
        tool::split(sql, allParams, ' ');
        //�ҵ����� tbName
        for (vector<string>::size_type i = 0; i < allParams.size() - 1; i++) {
            if (tool::compareNoCase(allParams[i], "FROM")) {
                string str = allParams[i + 1];

                string::iterator it;              //ָ��string��ĵ���������������Ϊָ��
                for (it = str.begin(); it <= str.end(); it++) {
                    if (*it == ';')
                        str.erase(it);          //ɾ��it����һ���ַ�
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
        //����begin
        if (params[2] != "*") {
            regex e("(\\w+),?"); //�ڶ���ƥ�䣬������ԺͲ��Ҫ�����ֵ ����1������2������3
            smatch sm2;
            string temp = params[2]; //��ȡ��ʽ���£�name,id,hobby
            int i = 2;
            params.erase(params.begin() + i);
            while (regex_search(temp, sm2, e)) {
                params.insert(params.begin() + i, sm2[1]);  //[0]�е������ж���
                temp = sm2.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
                i++;
            }
        }

        //����end
        string str = params[params.size() - 1];  //ȡ��Ҫ��ֵ�Ԫ��
        params.pop_back(); //ɾ�����һ��Ԫ��
        regex e2("(\\w+[ ]{0,}= {0,}\\w+),?;?");  //��ָ�ʽ ����1=value,����2=value;����3=value�� ƥ���� name=lisi    id=123
        smatch sm;
        while (regex_search(str, sm, e2)) {
            params.push_back(sm[1]);  //sm[0]����ƥ�䵽�������ַ���
            str = sm.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
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
    regex eExit("^exit;$", regex_constants::icase); //���Դ�Сд
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


