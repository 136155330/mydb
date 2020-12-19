
#include <iostream>
#include <vector>
#include "accessControl.h"

using namespace std;

Access::Access() {

    if (!fileTools.readDB(&accessDB, "SYS")) {
        cout << "SYS���ݿⲻ����";
    }

}

Access::~Access() {
    //д���ļ������г־û�����
    fileTools.writeDB(accessDB);
}

bool Access::checkLogin(string userName, string userPass) {

    //1������sql���
    // select * from user where userName = userName and userPass = userPass;
    //2����ѯ�Ƿ���ڼ�¼,���ھ͵�¼�ɹ�������ʧ��
    string sql = "select * from user where userName=" + userName + " and userPass=" + userPass + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT
    vector <vector<string>> answer = action.Find(message, accessDB);
    if (answer.size() == 0) {
        cout << "��¼ʧ�ܣ��û��������벻��ȷ��" << endl;
        return false;
    } else {
        //��¼��ǰ��¼�ɹ����û�������֮���Ȩ�޲�ѯ
        this->userName = userName;
        cout << "��¼�ɹ���" ;
        return true;
    }
}


bool Access::createUser(string userName, string userPass) {

    //1������sql���,��ѯ�û����Ƿ��Ѿ�����,���ھʹ���ʧ��
    //select * from user where userName=userName;
    string sql = "select * from user where userName=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT

    vector <vector<string>> answer = action.Find(message, accessDB);
    if (answer.size() != 0) {
        string errorMes = "�û�����ʧ�ܣ��û����Ѿ�����!";
        cout << errorMes << endl;
        return false;
    }

    //2������sql,�����û�
    //insert into user values(userName, userPass);
    sql = "insert into user values(" + userName + "," + userPass + ");";
    result = interpreter.analyse(sql);
    message.assign(result.begin() + 1, result.end());  //ȥ����INSERT_TB
    action.Add(message, &accessDB);
    fileTools.writeDB(accessDB);

    cout << "�û�" << userName << "�����ɹ���" << endl;
    return true;
}

bool Access::checkAccess(vector<string> params, DB * currentDB) {



    if(params[0]=="CREATE_DB" || params[0]=="DROP_DB" || params[0]=="USE_DB"){
        if(checkDbAccess(params)){
            return true;
        }else{
            cout<<"��û�д���Ȩ��"<<endl;
            return false;
        }

    }else if(params[0]=="CREATE_TABLE" || params[0]=="DROP_TB" || params[0]=="INSERT_TABLE"
                || params[0]=="DELETE" || params[0]=="UPDATE" || params[0]=="SELECT") {
        //��ͼ
        if(params[0]=="SELECT"){
            for(auto i:(*currentDB).views){
                if(params[1]==i.name){
                    return  true;
                }
            }
        }

        ///�жϱ��Ƿ����
        if(params[0]!="CREATE_TABLE"){
            vector<string> tableName = currentDB->showTables();
            bool exist = false;
            for(int i=0; i<tableName.size();i++){
                if(params[1]==tableName[i]){
                    exist=true;
                }
            }
            if(!exist){
                cout<<"��"<<params[1]<<"������!"<<endl;
                return false;
            }
        }

        if(checkTbAccess(params)){
            return true;
        }else{
            cout<<"��û�д���Ȩ��"<<endl;
            return false;
        }

    }else if(params[0]=="GRANT"){
        grantAccess(params);
        return false;
    }else if(params[0]=="REVOKE"){
        revokeAccess(params);
        return  false;
    }

    return true;
}


bool Access::checkDbAccess(vector<string> params) {

    if(tool::compareNoCase(params[0],"CREATE_DB")){
        string sql = "select * from dbAccess where dbName=" + params[1] + ";";
       // cout<<sql<<endl;
        vector <string> result = interpreter.analyse(sql);
        vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()!=0){
            cout<<"����ʧ�ܣ��ÿ��Ѿ�����!"<<endl;
            return false;
        }else{
            //�����¼
            sql = "insert into dbAccess values(" +params[1]+","+userName+");";
            result = interpreter.analyse(sql);

            message.assign(result.begin()+1,result.end());
            action.Add(message,&accessDB);
            fileTools.writeDB(accessDB);
            return  true;
        }
    }else if(tool::compareNoCase(params[0],"DROP_DB")){
        string sql = "select * from dbAccess where dbName=" + params[1] + " and owner=" + userName + ";";
        vector <string> result = interpreter.analyse(sql);
        int sz = result.size();
        vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()==0) {
            cout << "ɾ��ʧ�ܣ���û��Ȩ��!" << endl;
            return false;
        }else{

            sql = "delete from dbAccess where dbName="+params[1]+";";
            result = interpreter.analyse(sql);
            message.assign(result.begin()+1, result.end());
            action.Delete(message, &accessDB);
            fileTools.writeDB(accessDB);
            return true;
        }
    }else if(tool::compareNoCase(params[0],"USE_DB")){
        currentDbName = params[1];
        return  true;
    }

    return false;
}


bool Access::checkTbAccess(vector<string> params) {

    if(params[0].compare("CREATE_TABLE")==0 ){
        //����sql ��ѯ���û��Ƿ���Ȩ�޴�����
        //currentDbName = "EX";
        string sql = "select * from dbAccess where dbName=" + currentDbName +  " and owner="+userName+";";
        vector <string> result = interpreter.analyse(sql);
        vector <string> message(result.begin() + 1, result.end());
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()==0){
            cout<<"û��Ȩ�޴�����"<<endl;
            return false;
        }

        //��tbAccess�����¼

        sql = "insert into tbAccess values("+userName+","+params[1]+ ",1,1,1,1);";
        result = interpreter.analyse(sql);
        message.assign(result.begin()+1,result.end());
        action.Add(message, &accessDB);
        fileTools.writeDB(accessDB);
        return true;

    }else if(params[0].compare("DROP_TB")==0){
       // currentDbName = "EX";
        string sql = "select * from dbAccess where dbName=" + currentDbName+ " and owner=" +userName+";";
        vector <string> result = interpreter.analyse(sql);
        vector <string> message(result.begin() + 1, result.end());
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()==0){
            cout<<"û��Ȩ��"<<endl;
            return false;
        }
        //ɾ��tbAccess��¼
        sql = "delete from tbAccess where tbName="+params[1]+";";
        result = interpreter.analyse(sql);
        message.assign(result.begin()+1,result.end());
        action.Delete(message,  &accessDB);
        fileTools.writeDB(accessDB);
        return  true;

    }
    string type = "";
    if(params[0].compare("INSERT_TABLE")==0){
        type = "insert";
    }else if(params[0].compare("DELETE")==0){
        type = "delete";
    }else if(params[0].compare("UPDATE")==0){
        type = "update";
    }else if(params[0].compare("SELECT")==0){
        type = "select";
    }
    //��ѯ�Ƿ���Ȩ��

    string sql = "select * from tbAccess where userName="+userName+" and tbName="+params[1]+" and "+type+"=1;";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());
    vector <vector<string> > answer = action.Find(message, accessDB);
    if(answer.size()){
        return  true;
    }else{
        return false;
    }

}

bool Access::grantAccess(vector<string> params) {
    //1������sql���,��ѯ���û��ǲ��ǵ�ǰ���ݿⴴ���ߣ���������޷���Ȩ
    //select * from dbAccess where dbName=? and owner=?;
    //TODO currentDb
   // currentDbName = "sys";
    string sql = "select * from dbAccess where dbName=" + currentDbName + " and owner=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT
    vector <vector<string> > answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"��Ȩʧ�ܣ���û��Ȩ��!"<<endl;
        return false;
    }

    //2.����sql���,��ѯuserName�Լ�tbName �ļ�¼�Ƿ����
    sql = "select * from tbAccess where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1, result.end());
    answer = action.Find(message, accessDB);
    bool  exist = false;
    if(answer.size()){
        exist = true;
    }

    //3.�����¼������,����tbAccess�����¼��������Ȩ��������ڣ��Ըü�¼���и���
    if(exist){
        string pos="";
        if (tool::compareNoCase(params[1], "select")) {
            pos="select";
        } else if (tool::compareNoCase(params[1], "update")) {
            pos="update";
        } else if (tool::compareNoCase(params[1], "insert")) {
            pos="insert";
        }else if(tool::compareNoCase(params[1],"delete")){
            pos="delete";
        }else{
            cout << "�޷�����" << params[1] << "Ȩ��!" << endl;
            return false;
        }
        sql = "update tbAccess set "+ pos + "=1 where userName=" + params[3] + " and tbName=" + params[2]+";";
        result = interpreter.analyse(sql);
        message.assign(result.begin()+1,result.end());
        action.Update(message, &accessDB);
    }else {
        string values = "(" + params[3] + "," + params[2];
        if (tool::compareNoCase(params[1], "select")) {
            values += ",1,0,0,0)";
        } else if (tool::compareNoCase(params[1], "update")) {
            values += ",0,1,0,0)";
        } else if (tool::compareNoCase(params[1], "insert")) {
            values += ",0,0,1,0)";
        }else if(tool::compareNoCase(params[1],"delete")){
            values += ",0,0,0,1)";
        } else {
            cout << "��Ȩʧ�ܣ�������" << params[1] << "Ȩ��!" << endl;
            return false;
        }
        sql = "insert into tbAccess values" + values + ";";
        result = interpreter.analyse(sql);
        message.assign(result.begin() + 1, result.end());
        action.Add(message, &accessDB);
    }

    fileTools.writeDB(accessDB);
    cout<<"��Ȩ�ɹ���"<<endl;
    return  true;

}

bool Access::revokeAccess(vector <string> params) {

    //1������sql���,��ѯ���û��ǲ��ǵ�ǰ���ݿⴴ���ߣ���������޷��ջ�Ȩ��
    //select * from dbAccess where dbName=? and owner=?;

    string sql = "select * from dbAccess where dbName=" + currentDbName + " and owner=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //ȥ����SELECT
    vector <vector<string> > answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"�ջ�Ȩ��ʧ�ܣ���û��Ȩ�޽��иò���!"<<endl;
        return false;
    }

    //2.����sql���,��ѯuserName�Լ�tbName �ļ�¼�Ƿ����
    sql = "select * from tbAccess where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1, result.end());
    answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"�ջ�Ȩ��ʧ�ܣ��û�"<<params[3]<<"û�жԱ�"<<params[2]<<"�Ĳ���Ȩ��!"<<endl;
        return  false;
    }
    //3.����sql��䣬����Ȩ��
    string pos="";
    if (tool::compareNoCase(params[1], "select")) {
        pos="select";
    } else if (tool::compareNoCase(params[1], "update")) {
        pos="update";
    } else if (tool::compareNoCase(params[1], "insert")) {
        pos="insert";
    }else if(tool::compareNoCase(params[1],"delete")){
        pos="delete";
    } else {
        cout << "�ջ�Ȩ��ʧ�ܣ�������" << params[1] << "Ȩ��!" << endl;
        return false;
    }
    sql = "update tbAccess set "+ pos + "=0 where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1,result.end());
    action.Update(message, &accessDB);

    fileTools.writeDB(accessDB);
    cout<<"��Ȩ�ɹ���"<<endl;
    return true;
}

