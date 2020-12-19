//
// Created by dell on 2018/10/27.
//

#ifndef MYDB_FILEUTIL_H
#define MYDB_FILEUTIL_H

#include<bits/stdc++.h>
#include<iostream>
#include<string>
#include<regex>
#include <fstream>
#include <windows.h>
#include <io.h>  //判断文件夹是否存在：
#include <direct.h> //创建新的文件夹:
#include <stdlib.h>
#include "DB.h"
#include "tool.h"
#include "view.h"

using namespace std;

class fileUtil {
public:
    fileUtil() {};

    ~fileUtil() {};

    /**
     * 创建文件夹folder，只能创建一层
     * @param folder
     * @return   成功 true 失败  false
     */
    bool createDirectory(const std::string folder);

    vector<string> getDir(string path);

    bool inDir(string path, string fileName);

    /**
     * 将数据库中的数据写入文件filePath中
     * @param values
     * @param filePath
     * @return 成功 true 失败  false
     */
    bool writeValues(vector<vector<string>> values, string filePath);

    /**
     * 从 filePath中读取文件到getValues中
     * @param getValues
     * @param filePath
     * @return 成功 true 失败  false
     */
    bool readValues(vector<vector<string>> *getValues, string filePath);

    /**
     *
     * @param attributes
     * @param filePath
     * @return
     */
    bool writeAttributes(vector<Attribute> attributes, string filePath);

    /**
     *从filePath中读取数据到 attributes
     * @param attributes
     * @param filePath
     * @return
     */
    bool readAttributes(vector<Attribute> *attributes, string filePath);

    /**
     * 把一个数据库写入文件，固定存在D://allDB这个文件夹中,所以不用输入路径
     * @param db
     * @return
     */
    bool writeDB(DB db);

    /**
     * 从D://allDB中寻找与dbName相同的文件夹，并加载数据到db中。
     * @param db
     * @param dbName
     * @return
     */
    bool readDB(DB *db, string dbName);

    bool readDB2(string path, DB *db);

    void dropPath(string path);

    ///视图部分

    bool readDB3(string path, DB *db);

    bool createView(DB *db, view v);

    bool deleteView(DB *db, string vname);

    void getFiles(string path, vector<string> &files);
};

#endif //MYDB_FILEUTIL_H
