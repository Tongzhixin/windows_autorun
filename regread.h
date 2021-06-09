#ifndef REGREAD_H
#define REGREAD_H
#include <windows.h>

#include <winreg.h>
#include <stdio.h>
#include <tchar.h>
#include <convert.h>
#include <string>
#include <iostream>
#include <map>
#include <QDebug>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define HKLM HKEY_LOCAL_MACHINE
#define HKCU HKEY_CURRENT_USER

using namespace std;


map<string,string> read_reg(HKEY root_key,LPCSTR key_path);

map<int, string> read_subkey(HKEY root_key,LPCSTR key_path);
string readStubPath(HKEY aim_rootkey, LPCWSTR key_data);
string read_name(HKEY aim_rootkey, LPCWSTR key_data);
string read_imagepath(HKEY aim_rootkey, LPCWSTR key_data);
string read_description(HKEY aim_rootkey, LPCWSTR key_data);
DWORD read_type(HKEY aim_rootkey, LPCWSTR key_data);
DWORD read_start(HKEY aim_rootkey, LPCWSTR key_data);
map<int, string> read_subkey(HKEY root_key,LPCSTR key_path) {
    HKEY hKey;
    map<int, string> reg_map;
    char *tmp_value = NULL;


    if(RegOpenKeyExA(root_key,key_path,0, KEY_READ, &hKey) == ERROR_SUCCESS){
        TCHAR buf_class[MAX_PATH] = TEXT("");
        DWORD name_size, size_classname = MAX_PATH, cnt_subkey = 0,maxsize_subkey,maxclassname,cnt_value,maxsize_value,max_valuedata,size_security_descriptor;
        DWORD value_size,value_data_size,Type;
        DWORD i, retCode,retCode2;
        int count = 0;
        FILETIME lastWriteTime;
        retCode = RegQueryInfoKey(hKey,buf_class,&size_classname,NULL,&cnt_subkey,&maxsize_subkey,&maxclassname,&cnt_value,&maxsize_value,&max_valuedata,&size_security_descriptor,&lastWriteTime);
        if(retCode == ERROR_SUCCESS) {
            //cout<<"number of subkey: "<< cnt_subkey << '\n';
            for (i = 0;i<cnt_subkey;i++) {
                value_data_size = maxsize_subkey +1;

                TCHAR szValueData[MAX_KEY_LENGTH];
                retCode2 = RegEnumKeyEx(hKey,i,szValueData,&value_data_size,NULL,NULL,NULL,&lastWriteTime);

                if (retCode2 == ERROR_SUCCESS) {
                    //cout << "data:" << szValueData << '\n';
                    //qDebug()<<"subkey right";
                    tmp_value = TCHAR2char(szValueData);
                    string tmp = tmp_value;
                    reg_map[count] = tmp;
                    count++;
                    delete[] tmp_value;
                    tmp_value = NULL;
                }

            }
        }
        else {
            //cout << "error subkey" << endl;
        }
        RegCloseKey(hKey);
    }
    else {
         //cout << "error: open reg" << endl;
    }
    return reg_map;
}


map<string, string> read_reg(HKEY root_key,LPCSTR key_path){
    HKEY hKey;
    map<string, string> reg_map;
    char *tmp_value = NULL;
    char *tmp_name = NULL;
    TCHAR szValueDate[MAX_KEY_LENGTH];
    TCHAR szValueName[MAX_VALUE_NAME];
    //qDebug()<<"test "<< key_path << '\n';
    if(RegOpenKeyExA(root_key,key_path,0, KEY_READ, &hKey) == ERROR_SUCCESS){
        TCHAR buf_class[MAX_PATH] = TEXT("");
        DWORD name_size, size_classname = MAX_PATH, cnt_subkey = 0,maxsize_subkey,maxclassname,cnt_value,maxsize_value,max_valuedata,size_security_descriptor;
        DWORD value_size,value_data_size,Type;
        DWORD i, retCode;
        FILETIME lastWriteTime;
        retCode = RegQueryInfoKey(hKey,buf_class,&size_classname,NULL,&cnt_subkey,&maxsize_subkey,&maxclassname,&cnt_value,&maxsize_value,&max_valuedata,&size_security_descriptor,&lastWriteTime);
        if(retCode == ERROR_SUCCESS) {
            //qDebug()<<"number of value: "<< cnt_value << '\n';
            for (i = 0;i<cnt_value;i++) {
                value_data_size = max_valuedata +1;
                value_size = maxsize_value +1;
                //TCHAR* szValueName = (TCHAR*)malloc(value_size);
                //BYTE szValueDate[MAX_VALUE_NAME];
                //szValueDate[0] = '\0';
                retCode = RegEnumValue(hKey,i,szValueName,&value_size,NULL,&Type,LPBYTE(szValueDate),&value_data_size);
                if (retCode == ERROR_SUCCESS) {

                    tmp_name = TCHAR2char(szValueName);
                    tmp_value = TCHAR2char(szValueDate);
                    string str_tmp_name = tmp_name;
                    string str_tmp_value = tmp_value;
                    //qDebug() << "type:" << Type << "name:" << str_tmp_name.c_str() << "data:" << str_tmp_value.c_str() << '\n';
                    reg_map[str_tmp_name] = str_tmp_value;
                    delete[] tmp_name;
                    delete[] tmp_value;
                    tmp_name = NULL;
                    tmp_value = NULL;
                }

            }
        }
        else {
            //cout << "error subkey" << endl;
        }
    }
    else {
         //cout << "error: open reg" << endl;
    }
    return reg_map;
}

string read_imagepath(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD dwtype = 0;
    //LPBYTE lpvalue = NULL;
    TCHAR lpvalue[MAX_VALUE_NAME];
    DWORD dwsize = 0;

    long ret;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS)
    {
        RegQueryValueEx(cpp_key, _T("ImagePath"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        //lpvalue = (LPBYTE)malloc(dwsize);
        ret = RegQueryValueEx(cpp_key, _T("ImagePath"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        RegCloseKey(cpp_key);
    }
    char *tmp1 = TCHAR2char(lpvalue);
    string tmp2 = tmp1;
    delete [] tmp1;
    tmp1 = NULL;
    //qDebug()<<"imagepath:"<<tmp2.c_str();
    //cout << lpvalue << endl;
    return tmp2;
}
string read_name(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD dwtype = 0;
    //LPBYTE lpvalue = NULL;
    TCHAR lpvalue[MAX_VALUE_NAME];
    DWORD dwsize = 0;
    string tmp2 = "";
    long ret;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS)
    {

        //lpvalue = (LPBYTE)malloc(dwsize);
        ret = RegQueryValueEx(cpp_key, _T("Localized Name"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        ret = RegQueryValueEx(cpp_key, _T("Localized Name"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        if (ret !=ERROR_SUCCESS) {
            ret = RegQueryValueEx(cpp_key, _T("ComponentID"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
            ret = RegQueryValueEx(cpp_key, _T("ComponentID"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
            if (ret != ERROR_SUCCESS) {
                RegCloseKey(cpp_key);
                return tmp2;
            }

        }
        RegCloseKey(cpp_key);
    }
    char *tmp1 = TCHAR2char(lpvalue);
    tmp2 = tmp1;
    delete [] tmp1;
    tmp1 = NULL;
    //qDebug()<<"imagepath:"<<tmp2.c_str();
    //cout << lpvalue << endl;
    return tmp2;
}
string readStubPath(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD dwtype = 0;
    //LPBYTE lpvalue = NULL;
    TCHAR lpvalue[MAX_VALUE_NAME];
    DWORD dwsize = 0;

    long ret;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS)
    {
        ret = RegQueryValueEx(cpp_key, _T("StubPath"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        ret = RegQueryValueEx(cpp_key, _T("StubPath"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);

        RegCloseKey(cpp_key);
        if(ret!=ERROR_SUCCESS) {
            return "";
        }
    }
    char *tmp1 = TCHAR2char(lpvalue);
    string tmp2 = tmp1;
    delete [] tmp1;
    tmp1 = NULL;
    //qDebug()<<"imagepath:"<<tmp2.c_str();
    //cout << lpvalue << endl;
    return tmp2;
}
DWORD read_type(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD type = 100000;
    DWORD dwtype = REG_DWORD;
    DWORD dwvalue = sizeof(DWORD);
    DWORD ret,ret1;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS){
        ret1 = RegQueryValueEx(cpp_key, _T("Type"), 0, &dwtype, NULL, &dwvalue);
        ret1 = RegQueryValueEx(cpp_key, _T("Type"), 0, &dwtype, (LPBYTE)&type, &dwvalue);
        if(ret1 == ERROR_SUCCESS) {

            return type;
        } else {
            return 100000;
        }
        RegCloseKey(cpp_key);
    }
    return 100000;

    //cout << type << endl;

}

string read_description(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD dwtype = 0;
    TCHAR lpvalue[MAX_VALUE_NAME];
    DWORD dwsize = 0;

    long ret;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS)
    {
        RegQueryValueEx(cpp_key, _T("Description"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        ret = RegQueryValueEx(cpp_key, _T("Description"), 0, &dwtype, (LPBYTE)lpvalue, &dwsize);
        RegCloseKey(cpp_key);
        if (ret!=ERROR_SUCCESS) {
            return "";
        }
    }
    char *tmp1 = TCHAR2char(lpvalue);
    string tmp2 = tmp1;
    delete [] tmp1;
    tmp1 = NULL;
    //qDebug()<<"description:"<<tmp2.c_str();
    //cout << lpvalue << endl;
    return tmp2;
    //cout << lpvalue << endl;
}
DWORD read_start(HKEY aim_rootkey, LPCWSTR key_data)
{
    HKEY cpp_key;
    DWORD type = 100;
    DWORD dwtype = REG_DWORD;
    DWORD dwvalue = sizeof(DWORD);
    DWORD ret,ret1;
    ret = RegOpenKeyEx(aim_rootkey, key_data, 0, KEY_READ, &cpp_key);
    if (ret == ERROR_SUCCESS){
        ret1 = RegQueryValueEx(cpp_key, _T("Start"), 0, &dwtype, NULL, &dwvalue);
        ret1 = RegQueryValueEx(cpp_key, _T("Start"), 0, &dwtype, (LPBYTE)&type, &dwvalue);
        if(ret1 == ERROR_SUCCESS) {
            return type;
        } else {
            return 100;
        }
        RegCloseKey(cpp_key);
    }
    return 100;

    //cout << type << endl;

}



#endif // REGREAD_H
