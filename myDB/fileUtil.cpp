
#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include<regex>
#include <fstream>
#include <windows.h>
#include <io.h>  //判断文件夹是否存在：
#include <direct.h> //创建新的文件夹:
#include <stdlib.h>
#include "fileUtil.h"
#include "tool.h"
#include "view.h"
#include "viewOperation.h"

#define PATH_DELIMITER '\\'

bool fileUtil::createDirectory(const std::string folder) {
    std::string folder_builder;
    std::string sub;
    sub.reserve(folder.size());
    for (auto it = folder.begin(); it != folder.end(); ++it) {
        //cout << *(folder.end()-1) << endl;
        const char c = *it;
        sub.push_back(c);
        if (c == PATH_DELIMITER || it == folder.end() - 1) {
            folder_builder.append(sub);
            if (0 != ::_access(folder_builder.c_str(), 0)) {
                // this folder not exist
                if (0 != ::_mkdir(folder_builder.c_str())) {
                    // create failed
                    return false;
                }
            }
            sub.clear();
        }
    }
    return true;
}

vector<string> fileUtil::getDir(string path) {
    vector<string> temp;
    long hFile = 0;  //文件句柄
    struct _finddata_t fileInfo; //文件信息读取结构
    string pathName, exdName;
    // \\* 代表要遍历所有的类型
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //比较文件类型是否是文件夹
        return temp; //该数据库还没有表
    }
    do {
        Table tempTable;
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //过滤掉. ..
        {
            temp.push_back((string) fileInfo.name);
        }
        //待实现，释放之前tempTable 的空间
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return temp;
}

bool fileUtil::inDir(string path, string fileName) {
    long hFile = 0;  //文件句柄
    struct _finddata_t fileInfo; //文件信息读取结构
    string pathName, exdName;
    // \\* 代表要遍历所有的类型
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //比较文件类型是否是文件夹
        return false; //没有子文件
    }
    do {
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //过滤掉. ..
        {
            if (tool::compareNoCase((string) fileInfo.name, fileName)) {
                return true;
            }
        }
        //待实现，释放之前tempTable 的空间
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return false;
}

bool fileUtil::writeValues(vector<vector<string>> values, string filePath) {
    ofstream outfile;
    outfile.open(filePath, ios::trunc | ios::out);
    if (!outfile.is_open()) {
        return false;
    }
    if (!values.empty()) {
        for (int i = 0; i < values.size(); i++) {
            vector<string> temp = values[i];
            for (int j = 0; j < temp.size(); j++) {
                string str = temp[j];
                outfile << str << ',';
            }
            outfile << " ";
        }
    }
    outfile.close();
    return true;
}

bool fileUtil::readValues(vector<vector<string>> *getValues, string filePath) {
    ifstream infile;
    vector<string> getValue;  //临时存储一行数据
    string flag;
    string temp;           //flag temp 用作临时存储一行数据
    regex e("([^ ]+?),"); //第二次匹配，拆分属性和拆分要插入的值 属性1，属性2，属性3
    smatch sm2;
    infile.open(filePath, ios::in);
    if (!infile.is_open()) {
        return false;
    }
    while (1) {
        infile >> flag;
        if (infile.eof()) {
            break;
        }
        temp = flag;
        while (regex_search(temp, sm2, e)) { //分解字符串，放入vector<string>
            string str = sm2[1];
            getValue.push_back(str);  //[0]中的数据有逗号
            temp = sm2.suffix();//获取还没有匹配到的部分，再次匹配
        }
        getValues->push_back(getValue);  //把vector<string>一行数据放入vector<vector<string>>
        getValue.clear();              //清空，接受新的一行
    }
    infile.close();
    return true;
}

bool fileUtil::writeAttributes(vector<Attribute> attributes, string filePath) {
    ofstream out(filePath.c_str(), ios::trunc);
    if (!out.is_open()) {
        return false;
    }
    for (int i = 0; i < attributes.size(); i++) {
        out << attributes[i];   //  有重载<<操作符
    }
    out.close();
    return true;
}

bool fileUtil::readAttributes(vector<Attribute> *attributes, string filePath) {
    ifstream in(filePath.c_str());
    if (!in.is_open()) {
        return false;
    }
    Attribute temp = Attribute();
    while (1) {
        in >> temp;
        if (in.eof())
            break;
        attributes->push_back(temp);  //将一个属性存入属性集合
    }
    in.close();
}

bool fileUtil::writeDB(DB db) {
    string filePath = "D://allDB";
    //判断是否存在D://allDB 若不存在，创建它
    if (0 != ::_access(filePath.c_str(), 0)) {//文件存在返回0，否则返回-1
        // this folder not exist
        if (0 != ::_mkdir(filePath.c_str())) {
            // create failed
            return false;
        }
    }
    filePath = filePath + "//" + db.name;
    if (0 != ::_mkdir(filePath.c_str())) {
        // create failed
        if (!createDirectory(filePath)) {
            return false;
        }

    }
    if (db.tables.empty()) {  //如果没有表，结束函数
        return true;
    }

    for (int i = 0; i < db.tables.size(); i++) {
        Table curTable = db.tables[i];
        string tempPath = filePath + "//" + curTable.table_name;
        if (0 != ::_mkdir(tempPath.c_str())) {
            if (!createDirectory(tempPath)) {
                // create failed
                return false;
            }
        }

        string attriPath = tempPath + "//attributes.txt";  //表的字段信息写入文件
        if (!writeAttributes(curTable.attributes, attriPath)) {
            return false;
        }
        string valPath = tempPath + "//values.txt";
        if (!writeValues(curTable.values, valPath)) {
            return false;
        }
    }
    return true;
}

bool fileUtil::readDB(DB *db, string dbName) {

    string root = "D://allDB";
    if (0 == ::_access(root.c_str(), 0)) { //文件存在返回0，否则返回-1
        db->name = dbName;
        string path = root + "//" + dbName;
        if (0 == ::_access(path.c_str(), 0)) {

            if (readDB2(path, db)) {
                readDB3(path, db);

                return true;
            }
        }
    }
    return false;
}

bool fileUtil::readDB2(string path, DB *db) {
    fileUtil tool;
    string tempPath;
    long hFile = 0;  //文件句柄
    struct _finddata_t fileInfo; //文件信息读取结构
    string pathName, exdName;
    // \\* 代表要遍历所有的类型
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //比较文件类型是否是文件夹
        return true; //该数据库还没有表
    }
    do {
        Table tempTable;
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //过滤掉. ..
        {
            tempTable.table_name = (string) fileInfo.name;
            if (tempTable.table_name.find(".view") != string::npos) continue;
            tempPath = path + "//" + tempTable.table_name + "//attributes.txt";
            tool.readAttributes(&(tempTable.attributes), tempPath);
            tempPath = path + "//" + tempTable.table_name + "//values.txt";
            tool.readValues(&(tempTable.values), tempPath);
            db->tables.push_back(tempTable);
        }
        //待实现，释放之前tempTable 的空间
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return true;
}

void fileUtil::dropPath(string path) {
    if (access(path.c_str(), 0) == -1)//参数为0时表示检查文件的存在性，如果文件存在，返回0，不存在，返回-1
    {
        cout << "不存在，无法删除";
        return;
    }
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //如果是目录,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    dropPath(p.assign(path).append("\\").append(fileinfo.name));
            } else {
                string removePath = p.assign(path).append("\\").append(fileinfo.name);
                remove(removePath.c_str());
                //files.push_back(p.assign(path).append("\\").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        rmdir(path.c_str());
        _findclose(hFile);
    }
}

//视图文件的创建及关闭

bool fileUtil::createView(DB *db, view v) {

    string path = "D://allDB";
    path += "//" + db->name;
    path += "//" + v.name + ".view";

    ofstream out;

    out.open(path);

    for (auto i = v.message.begin(); i != v.message.end(); i++) {
        out << *i << endl;
    }

    out.close();

    return true;
}

bool fileUtil::deleteView(DB *db, string vname) {

    string path = "D://allDB";
    path += "//" + db->name;
    path += "//" + vname + ".view";

    remove(path.data());

    return true;
}

bool fileUtil::readDB3(string path, DB *db) {
    vector<string> files;

    ifstream input;
    string name;
    vector<string> message;
    string temp;

    getFiles(path, files);

    for (int i = 0; i < files.size(); i++) {
        int len = files[i].size();

        string k = files[i];

        //cout<<" test : file name = " << k << endl;

        if (k.find("view") == k.npos) {
            continue;
        }

        auto be = k.rfind("/");
        auto en = k.rfind(".");

/*
            cout<<"test 3:"<<endl;
            for(auto i = be ;i != en;i ++){
                cout<<k[i];
            }
            cout<<endl;
*/
        name.assign(k.begin() + be + 1, k.begin() + en);

//           cout<<" test2 : view name = " <<name << endl;

        input.open(files[i]);

        message.clear();

        while (input >> temp) {
            message.push_back(temp);
        }

        view v(name, message);

        (db->views).push_back(v);
    }


}

void fileUtil::getFiles(string path, vector<string> &files) {
    //文件句柄
    long hFile = 0;
    //文件信息
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //如果是目录,迭代之
            //如果不是,加入列表
            if ((fileinfo.attrib & _A_SUBDIR)) {
                if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
                    getFiles(p.assign(path).append("//").append(fileinfo.name), files);
            } else {
                files.push_back(p.assign(path).append("//").append(fileinfo.name));
            }
        } while (_findnext(hFile, &fileinfo) == 0);
        _findclose(hFile);
    }

}