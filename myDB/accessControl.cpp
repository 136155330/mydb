
#include <iostream>
#include <vector>
#include "accessControl.h"

using namespace std;

Access::Access() {

    if (!fileTools.readDB(&accessDB, "SYS")) {
        cout << "SYS数据库不存在";
    }

}

Access::~Access() {
    //写入文件，进行持久化保存
    fileTools.writeDB(accessDB);
}

bool Access::checkLogin(string userName, string userPass) {

    //1、构造sql语句
    // select * from user where userName = userName and userPass = userPass;
    //2、查询是否存在记录,存在就登录成功，否则失败
    string sql = "select * from user where userName=" + userName + " and userPass=" + userPass + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT
    vector <vector<string>> answer = action.Find(message, accessDB);
    if (answer.size() == 0) {
        cout << "登录失败，用户名或密码不正确！" << endl;
        return false;
    } else {
        //记录当前登录成功的用户，用于之后的权限查询
        this->userName = userName;
        cout << "登录成功！" ;
        return true;
    }
}


bool Access::createUser(string userName, string userPass) {

    //1、构造sql语句,查询用户名是否已经存在,存在就创建失败
    //select * from user where userName=userName;
    string sql = "select * from user where userName=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT

    vector <vector<string>> answer = action.Find(message, accessDB);
    if (answer.size() != 0) {
        string errorMes = "用户创建失败，用户名已经存在!";
        cout << errorMes << endl;
        return false;
    }

    //2、构造sql,创建用户
    //insert into user values(userName, userPass);
    sql = "insert into user values(" + userName + "," + userPass + ");";
    result = interpreter.analyse(sql);
    message.assign(result.begin() + 1, result.end());  //去除掉INSERT_TB
    action.Add(message, &accessDB);
    fileTools.writeDB(accessDB);

    cout << "用户" << userName << "创建成功！" << endl;
    return true;
}

bool Access::checkAccess(vector<string> params, DB * currentDB) {



    if(params[0]=="CREATE_DB" || params[0]=="DROP_DB" || params[0]=="USE_DB"){
        if(checkDbAccess(params)){
            return true;
        }else{
            cout<<"您没有此项权限"<<endl;
            return false;
        }

    }else if(params[0]=="CREATE_TABLE" || params[0]=="DROP_TB" || params[0]=="INSERT_TABLE"
                || params[0]=="DELETE" || params[0]=="UPDATE" || params[0]=="SELECT") {
        //视图
        if(params[0]=="SELECT"){
            for(auto i:(*currentDB).views){
                if(params[1]==i.name){
                    return  true;
                }
            }
        }

        ///判断表是否存在
        if(params[0]!="CREATE_TABLE"){
            vector<string> tableName = currentDB->showTables();
            bool exist = false;
            for(int i=0; i<tableName.size();i++){
                if(params[1]==tableName[i]){
                    exist=true;
                }
            }
            if(!exist){
                cout<<"表"<<params[1]<<"不存在!"<<endl;
                return false;
            }
        }

        if(checkTbAccess(params)){
            return true;
        }else{
            cout<<"您没有此项权限"<<endl;
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
        vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()!=0){
            cout<<"创建失败，该库已经存在!"<<endl;
            return false;
        }else{
            //插入记录
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
        vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()==0) {
            cout << "删除失败，您没有权限!" << endl;
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
        //构造sql 查询该用户是否有权限创建表
        //currentDbName = "EX";
        string sql = "select * from dbAccess where dbName=" + currentDbName +  " and owner="+userName+";";
        vector <string> result = interpreter.analyse(sql);
        vector <string> message(result.begin() + 1, result.end());
        vector <vector<string> > answer = action.Find(message, accessDB);
        if(answer.size()==0){
            cout<<"没有权限创建表！"<<endl;
            return false;
        }

        //在tbAccess插入记录

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
            cout<<"没有权限"<<endl;
            return false;
        }
        //删除tbAccess记录
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
    //查询是否有权限

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
    //1、构造sql语句,查询该用户是不是当前数据库创建者，如果不是无法授权
    //select * from dbAccess where dbName=? and owner=?;
    //TODO currentDb
   // currentDbName = "sys";
    string sql = "select * from dbAccess where dbName=" + currentDbName + " and owner=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT
    vector <vector<string> > answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"授权失败，您没有权限!"<<endl;
        return false;
    }

    //2.构造sql语句,查询userName以及tbName 的记录是否存在
    sql = "select * from tbAccess where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1, result.end());
    answer = action.Find(message, accessDB);
    bool  exist = false;
    if(answer.size()){
        exist = true;
    }

    //3.如果记录不存在,则在tbAccess插入记录，进行授权；如果存在，对该记录进行更新
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
            cout << "无法授予" << params[1] << "权限!" << endl;
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
            cout << "授权失败，不存在" << params[1] << "权限!" << endl;
            return false;
        }
        sql = "insert into tbAccess values" + values + ";";
        result = interpreter.analyse(sql);
        message.assign(result.begin() + 1, result.end());
        action.Add(message, &accessDB);
    }

    fileTools.writeDB(accessDB);
    cout<<"授权成功！"<<endl;
    return  true;

}

bool Access::revokeAccess(vector <string> params) {

    //1、构造sql语句,查询该用户是不是当前数据库创建者，如果不是无法收回权限
    //select * from dbAccess where dbName=? and owner=?;

    string sql = "select * from dbAccess where dbName=" + currentDbName + " and owner=" + userName + ";";
    vector <string> result = interpreter.analyse(sql);
    vector <string> message(result.begin() + 1, result.end());  //去除掉SELECT
    vector <vector<string> > answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"收回权限失败，您没有权限进行该操作!"<<endl;
        return false;
    }

    //2.构造sql语句,查询userName以及tbName 的记录是否存在
    sql = "select * from tbAccess where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1, result.end());
    answer = action.Find(message, accessDB);
    if(answer.size()==0){
        cout<<"收回权限失败，用户"<<params[3]<<"没有对表"<<params[2]<<"的操作权限!"<<endl;
        return  false;
    }
    //3.构造sql语句，更新权限
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
        cout << "收回权限失败，不存在" << params[1] << "权限!" << endl;
        return false;
    }
    sql = "update tbAccess set "+ pos + "=0 where userName=" + params[3] + " and tbName=" + params[2]+";";
    result = interpreter.analyse(sql);
    message.assign(result.begin()+1,result.end());
    action.Update(message, &accessDB);

    fileTools.writeDB(accessDB);
    cout<<"收权成功！"<<endl;
    return true;
}

