#ifndef CONVERT_H
#define CONVERT_H

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <string>
#include <windows.h>
#include <QString>

char* TCHAR2char(const TCHAR* STR);
TCHAR* char2TCHAR(const char* str);
std::string TCHAR2STRING(TCHAR *STR);
char* QString2char(QString qstr);
std::string DwordToString(DWORD val);
char* TCHAR2char(const TCHAR* STR)
{
    int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, FALSE);

    char* str = new char[sizeof(char) * size];

    WideCharToMultiByte(CP_ACP, 0, STR, -1, str, size, NULL, FALSE);

    return str;
}

TCHAR* char2TCHAR(const char* str)
{

    int size = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, 0);

    TCHAR* retStr = new TCHAR[size * sizeof(TCHAR)];

    MultiByteToWideChar(CP_ACP, 0, str, -1, retStr, size);

    return retStr;
}

// convert TCHAR to string
std::string TCHAR2STRING(TCHAR *STR)
{
    int size = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);

    char* chRtn  =new char[size*sizeof(char)];

    WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, size, NULL, NULL);

    std::string str(chRtn);
    delete[] chRtn;
    return str;
}

char* QString2char(QString qstr)
{
    char* ch = new char;
    QByteArray byte = qstr.toLocal8Bit();
    ch = byte.data();
    return ch;
}

std::string DwordToString(DWORD val)
{
    std::string cur_str = std::to_string(val);
    return cur_str;
}
#endif // CONVERT_H
