
#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include<regex>
#include <fstream>
#include <windows.h>
#include <io.h>  //�ж��ļ����Ƿ���ڣ�
#include <direct.h> //�����µ��ļ���:
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
    long hFile = 0;  //�ļ����
    struct _finddata_t fileInfo; //�ļ���Ϣ��ȡ�ṹ
    string pathName, exdName;
    // \\* ����Ҫ�������е�����
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //�Ƚ��ļ������Ƿ����ļ���
        return temp; //�����ݿ⻹û�б�
    }
    do {
        Table tempTable;
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //���˵�. ..
        {
            temp.push_back((string) fileInfo.name);
        }
        //��ʵ�֣��ͷ�֮ǰtempTable �Ŀռ�
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return temp;
}

bool fileUtil::inDir(string path, string fileName) {
    long hFile = 0;  //�ļ����
    struct _finddata_t fileInfo; //�ļ���Ϣ��ȡ�ṹ
    string pathName, exdName;
    // \\* ����Ҫ�������е�����
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //�Ƚ��ļ������Ƿ����ļ���
        return false; //û�����ļ�
    }
    do {
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //���˵�. ..
        {
            if (tool::compareNoCase((string) fileInfo.name, fileName)) {
                return true;
            }
        }
        //��ʵ�֣��ͷ�֮ǰtempTable �Ŀռ�
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
    vector<string> getValue;  //��ʱ�洢һ������
    string flag;
    string temp;           //flag temp ������ʱ�洢һ������
    regex e("([^ ]+?),"); //�ڶ���ƥ�䣬������ԺͲ��Ҫ�����ֵ ����1������2������3
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
        while (regex_search(temp, sm2, e)) { //�ֽ��ַ���������vector<string>
            string str = sm2[1];
            getValue.push_back(str);  //[0]�е������ж���
            temp = sm2.suffix();//��ȡ��û��ƥ�䵽�Ĳ��֣��ٴ�ƥ��
        }
        getValues->push_back(getValue);  //��vector<string>һ�����ݷ���vector<vector<string>>
        getValue.clear();              //��գ������µ�һ��
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
        out << attributes[i];   //  ������<<������
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
        attributes->push_back(temp);  //��һ�����Դ������Լ���
    }
    in.close();
}

bool fileUtil::writeDB(DB db) {
    string filePath = "D://allDB";
    //�ж��Ƿ����D://allDB �������ڣ�������
    if (0 != ::_access(filePath.c_str(), 0)) {//�ļ����ڷ���0�����򷵻�-1
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
    if (db.tables.empty()) {  //���û�б���������
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

        string attriPath = tempPath + "//attributes.txt";  //����ֶ���Ϣд���ļ�
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
    if (0 == ::_access(root.c_str(), 0)) { //�ļ����ڷ���0�����򷵻�-1
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
    long hFile = 0;  //�ļ����
    struct _finddata_t fileInfo; //�ļ���Ϣ��ȡ�ṹ
    string pathName, exdName;
    // \\* ����Ҫ�������е�����
    if ((hFile = _findfirst(pathName.assign(path).append("\\*").c_str(), &fileInfo)) == -1) { //�Ƚ��ļ������Ƿ����ļ���
        return true; //�����ݿ⻹û�б�
    }
    do {
        Table tempTable;
        if ((strcmp(fileInfo.name, ".") != 0) && (strcmp(fileInfo.name, "..") != 0))  //���˵�. ..
        {
            tempTable.table_name = (string) fileInfo.name;
            if (tempTable.table_name.find(".view") != string::npos) continue;
            tempPath = path + "//" + tempTable.table_name + "//attributes.txt";
            tool.readAttributes(&(tempTable.attributes), tempPath);
            tempPath = path + "//" + tempTable.table_name + "//values.txt";
            tool.readValues(&(tempTable.values), tempPath);
            db->tables.push_back(tempTable);
        }
        //��ʵ�֣��ͷ�֮ǰtempTable �Ŀռ�
    } while (_findnext(hFile, &fileInfo) == 0);
    _findclose(hFile);
    return true;
}

void fileUtil::dropPath(string path) {
    if (access(path.c_str(), 0) == -1)//����Ϊ0ʱ��ʾ����ļ��Ĵ����ԣ�����ļ����ڣ�����0�������ڣ�����-1
    {
        cout << "�����ڣ��޷�ɾ��";
        return;
    }
    //�ļ����
    long hFile = 0;
    //�ļ���Ϣ
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //�����Ŀ¼,����֮
            //�������,�����б�
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

//��ͼ�ļ��Ĵ������ر�

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
    //�ļ����
    long hFile = 0;
    //�ļ���Ϣ
    struct _finddata_t fileinfo;
    string p;
    if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1) {
        do {
            //�����Ŀ¼,����֮
            //�������,�����б�
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