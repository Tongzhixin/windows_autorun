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


map<char*,char*> read_reg(HKEY root_key,LPCSTR key_path);

map<int, char*> read_subkey(HKEY root_key,LPCSTR key_path);
LPBYTE read_imagepath(HKEY aim_rootkey, LPCSTR key_data);


map<int, char*> read_subkey(HKEY root_key,LPCSTR key_path) {
    HKEY hKey;
    map<int, char*> reg_map;
    char *tmp_value;

    if(RegOpenKeyExA(root_key,key_path,0, KEY_READ, &hKey) == ERROR_SUCCESS){
        TCHAR buf_class[MAX_PATH] = TEXT("");
        DWORD name_size, size_classname = MAX_PATH, cnt_subkey = 0,maxsize_subkey,maxclassname,cnt_value,maxsize_value,max_valuedata,size_security_descriptor;
        DWORD value_size,value_data_size,Type;
        DWORD i, retCode;
        int count = 0;
        FILETIME lastWriteTime;
        retCode = RegQueryInfoKey(hKey,buf_class,&size_classname,NULL,&cnt_subkey,&maxsize_subkey,&maxclassname,&cnt_value,&maxsize_value,&max_valuedata,&size_security_descriptor,&lastWriteTime);
        if(retCode == ERROR_SUCCESS) {
            cout<<"number of subkey: "<< cnt_subkey << '\n';
            for (i = 0;i<cnt_subkey;i++) {
                value_data_size = maxsize_subkey +1;

                TCHAR* szValueData = (TCHAR*)malloc(value_data_size);

                retCode = RegEnumKeyEx(hKey,i,szValueData,&value_data_size,NULL,NULL,NULL,NULL);
                if (retCode == ERROR_SUCCESS) {
                    cout << "data:" << szValueData << '\n';
                    tmp_value = TCHAR2char(szValueData);
                    reg_map[count] = tmp_value;
                    count++;
                }
            }
        }
        else {
            cout << "error subkey" << endl;
        }
        RegCloseKey(hKey);
    }
    else {
         cout << "error: open reg" << endl;
    }
    return reg_map;
}


map<char*, char*> read_reg(HKEY root_key,LPCSTR key_path){
    HKEY hKey;
    map<char*, char*> reg_map;
    char *tmp_value;
    TCHAR szValueDate[MAX_VALUE_NAME];
    qDebug()<<"test "<< key_path << '\n';
    if(RegOpenKeyExA(root_key,key_path,0, KEY_READ, &hKey) == ERROR_SUCCESS){
        TCHAR buf_class[MAX_PATH] = TEXT("");
        DWORD name_size, size_classname = MAX_PATH, cnt_subkey = 0,maxsize_subkey,maxclassname,cnt_value,maxsize_value,max_valuedata,size_security_descriptor;
        DWORD value_size,value_data_size,Type;
        DWORD i, retCode;
        FILETIME lastWriteTime;
        retCode = RegQueryInfoKey(hKey,buf_class,&size_classname,NULL,&cnt_subkey,&maxsize_subkey,&maxclassname,&cnt_value,&maxsize_value,&max_valuedata,&size_security_descriptor,&lastWriteTime);
        if(retCode == ERROR_SUCCESS) {
            qDebug()<<"number of value: "<< cnt_value << '\n';
            for (i = 0;i<cnt_value;i++) {
                value_data_size = max_valuedata +1;
                value_size = maxsize_value +1;
                TCHAR* szValueName = (TCHAR*)malloc(value_size);
                //BYTE szValueDate[MAX_VALUE_NAME];
                //szValueDate[0] = '\0';
                retCode = RegEnumValue(hKey,i,szValueName,&value_size,NULL,&Type,LPBYTE(szValueDate),&value_data_size);
                if (retCode == ERROR_SUCCESS) {

                    tmp_value = TCHAR2char(szValueName);
                    char *tmp2 = TCHAR2char(szValueDate);
                    qDebug() << "type:" << Type << "name:" << tmp_value << "data:" << tmp2 << '\n';
                    reg_map[tmp_value] = tmp2;
                }

            }
        }
        else {
            cout << "error subkey" << endl;
        }
    }
    else {
         cout << "error: open reg" << endl;
    }
    return reg_map;
}

LPBYTE read_imagepath(HKEY aim_rootkey, LPCSTR key_data)
{
    HKEY cpp_key;
    DWORD dwtype = 0;
    LPBYTE lpvalue = NULL;
    DWORD dwsize = 0;

    long ret;
    ret = RegOpenKeyExA(aim_rootkey, key_data, 0, KEY_QUERY_VALUE, &cpp_key);
    if (ret == ERROR_SUCCESS)
    {
        RegQueryValueEx(cpp_key, _T("ImagePath"), 0, &dwtype, lpvalue, &dwsize);
        lpvalue = (LPBYTE)malloc(dwsize);
        ret = RegQueryValueEx(cpp_key, _T("ImagePath"), 0, &dwtype, lpvalue, &dwsize);
        RegCloseKey(cpp_key);
    }
    char *tmp2 = (char*)lpvalue;
    qDebug()<<"imagepath:"<<tmp2;
    //cout << lpvalue << endl;
    return lpvalue;
}






#endif // REGREAD_H
