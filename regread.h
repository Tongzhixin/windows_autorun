#ifndef REGREAD_H
#define REGREAD_H
#include <windows.h>

#include <winreg.h>
#include <stdio.h>
#include <tchar.h>
//#include <convert.h>
#include <string>
#include <iostream>
#include <map>

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383
#define HKLM HKEY_LOCAL_MACHINE
#define HKCU HKEY_CURRENT_USER

using namespace std;

map<TCHAR*,LPBYTE> read_reg(HKEY root_key,LPCSTR key_path);

map<int, TCHAR*> read_subkey(HKEY root_key,LPCSTR key_path);


map<int, TCHAR*> read_subkey(HKEY root_key,LPCSTR key_path) {
    HKEY hKey;
    map<int, TCHAR*> reg_map;

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
                    reg_map[count] = szValueData;
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


map<TCHAR*, LPBYTE> read_reg(HKEY root_key,LPCSTR key_path){
    HKEY hKey;
    map<TCHAR*, LPBYTE> reg_map;

    if(RegOpenKeyExA(root_key,key_path,0, KEY_READ, &hKey) == ERROR_SUCCESS){
        TCHAR buf_class[MAX_PATH] = TEXT("");
        DWORD name_size, size_classname = MAX_PATH, cnt_subkey = 0,maxsize_subkey,maxclassname,cnt_value,maxsize_value,max_valuedata,size_security_descriptor;
        DWORD value_size,value_data_size,Type;
        DWORD i, retCode;
        FILETIME lastWriteTime;
        retCode = RegQueryInfoKey(hKey,buf_class,&size_classname,NULL,&cnt_subkey,&maxsize_subkey,&maxclassname,&cnt_value,&maxsize_value,&max_valuedata,&size_security_descriptor,&lastWriteTime);
        if(retCode == ERROR_SUCCESS) {
            cout<<"number of value: "<< cnt_value << '\n';
            for (i = 0;i<cnt_value;i++) {
                value_data_size = max_valuedata +1;
                value_size = maxsize_value +1;
                TCHAR* szValueName = (TCHAR*)malloc(value_size);
                LPBYTE szValueDate = (LPBYTE)malloc(value_data_size);
                retCode = RegEnumValue(hKey,i,szValueName,&value_size,NULL,&Type,szValueDate,&value_data_size);
                if (retCode == ERROR_SUCCESS) {
                    cout << "type:" << Type << "name:" << szValueName << "data:" << szValueDate << '\n';
                    reg_map[szValueName] = szValueDate;
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







#endif // REGREAD_H
