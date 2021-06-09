#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "regread.h"
#include "signature.h"
#include "convert.h"
#include "description.h"
#include "readTask.h"
#define logon { "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx", \
                "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"}
#define services "SYSTEM\\CurrentControlSet\\Services"
#define knowndlls "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\KnownDlls"
#define activex "SOFTWARE\\Microsoft\\Active Setup\\Installed Components"
void drawHeader(QTableWidget *table, int rowIndex, QString path);
void initTablePara(QTableWidget* t);
void initTableLogon(QTableWidget* t, HKEY rootKey, QString path);
void initTableService(QTableWidget* t, HKEY rootKey, QString path );
void initTableTask(QTableWidget* t);
void initTableDll(QTableWidget* t, HKEY rootKey, QString path );
void initTableDriver(QTableWidget* t,HKEY rootKey, QString path);
void setTableItem(QTableWidget* t, int rowIndex, QString imagePath, QString description);
void RepairString(QString *str);
void initTableLogonDir(QTableWidget* t);
void initTableActiveX(QTableWidget* t, HKEY rootKey, QString path);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    initTablePara(ui->tableWidget_logon);
    initTablePara(ui->tableWidget_service);
    initTablePara(ui->tableWidget_driver);
    initTablePara(ui->tableWidget_task);
    initTablePara(ui->tableWidget_dll);
    initTablePara(ui->tableWidget_active);
    ui->tabWidget->setCurrentIndex(0);

    foreach(QString path, QStringList logon)
    {
        initTableLogon(ui->tableWidget_logon, HKLM, path);
        initTableLogon(ui->tableWidget_logon, HKCU, path);
    }

    initTableDriver(ui->tableWidget_driver,HKLM,services);
    initTableService(ui->tableWidget_service,HKLM,services);

    initTableTask(ui->tableWidget_task);
    initTableDll(ui->tableWidget_dll,HKLM,knowndlls);
    initTableLogonDir(ui->tableWidget_logon);

    initTableActiveX(ui->tableWidget_active,HKLM,activex);

}

MainWindow::~MainWindow()
{
    delete ui;
}
void test(){
    /*
    HKEY rootKey = HKLM;
    QString path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    map<string, string> regMap = read_reg(rootKey, path.toLocal8Bit());

    map<string, string>::iterator regIt = regMap.begin();
    QString key, imagePath;
    while(regIt != regMap.end()) {
        key = QString::fromStdString(regIt->first);
        imagePath = QString::fromStdString(regIt->second);
        qDebug()<<key<<":"<<imagePath;
        regIt++;
    }
    */

}

void setTableItem(QTableWidget* t, int rowIndex, QString imagePath, QString description) {
    t->setRowHeight(rowIndex, 35);
    QTableWidgetItem* imagePathItem = new QTableWidgetItem(imagePath);
    t->setItem(rowIndex, 4, imagePathItem);

    QFileInfo file_info(imagePath.toLocal8Bit().data());
    QString timestamp = file_info.created().toString("yyyy/MM/dd hh:mm");

    QFileIconProvider iconProvider;
    QIcon icon = iconProvider.icon(file_info);

    QString *buf_description = new QString;
    if(description.startsWith("@")) {
        GetDllDescription(description, buf_description);
    } else {
        GetFileDescription(imagePath, buf_description);
    }
    QString *buf_verify = new QString;
    VerifyEmbeddedSignature(imagePath.toStdWString().c_str(), buf_verify);
    QString isVerified = *buf_verify;

    QString *buf_pub = new QString("");

    TCHAR* file_path = char2TCHAR(imagePath.toStdString().c_str());
    GetSignaturePublisher(file_path, buf_pub);
    QString publisher = isVerified + " " + *buf_pub;

    QTableWidgetItem* timeStampItem = new QTableWidgetItem(timestamp);
    t->setItem(rowIndex, 5, timeStampItem);

    QTableWidgetItem* iconItem = new QTableWidgetItem;
    iconItem->setIcon(icon);
    t->setItem(rowIndex, 0, iconItem);
    QTableWidgetItem* descriptionItem = new QTableWidgetItem(*buf_description);
    t->setItem(rowIndex, 2, descriptionItem);
    QTableWidgetItem* publisherItem = new QTableWidgetItem(publisher);
    t->setItem(rowIndex, 3, publisherItem);
    delete buf_description;
    delete buf_verify;
    delete buf_pub;
    delete [] file_path;
    file_path = NULL;
}
void initTableLogon(QTableWidget* t, HKEY rootKey, QString path){
    map<string, string> regMap = read_reg(rootKey, path.toLocal8Bit());

    map<string, string>::iterator regIt;
    int rowIndex = t->rowCount();
    QString key, imagePath;
    regIt = regMap.begin();
    if (regMap.empty()) {
        //std::cout<<path.toStdString()<<"empty"<<'\n';
        return;
    }
    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);
    while(regIt != regMap.end()) {
        rowIndex++;
        t->setRowCount(rowIndex+1);
        key = QString::fromStdString(regIt->first);
        imagePath = QString::fromStdString(regIt->second);
        QTableWidgetItem* keyItem = new QTableWidgetItem(key);
        t->setItem(rowIndex, 1, keyItem);
        RepairString(&imagePath);
        setTableItem(t,rowIndex,imagePath,"");
        regIt++;
    }
}
void initTableLogonDir(QTableWidget* t) {
    map<string, map<string,string>> allData;
    createConnectToStart(allData);
    map<string, map<string,string>>::iterator fIt = allData.begin();
    int rowIndex;
    QString key, imagePath;
    while (fIt != allData.end()) {
        rowIndex = t->rowCount();
        drawHeader(t, rowIndex, QString::fromStdString(fIt->first));
        map<string,string> subMap = fIt->second;
        map<string, string>::iterator subIt = subMap.begin();
        while (subIt != subMap.end()) {
            rowIndex++;
            t->setRowCount(rowIndex+1);
            key = QString::fromStdString(subIt->first);
            imagePath = QString::fromStdString(subIt->second);
            if(imagePath.endsWith("lnk") || imagePath.endsWith("LNK")) {
                QFileInfo fileInfo(imagePath);
                imagePath = fileInfo.canonicalFilePath();
            }
            if(imagePath.endsWith("ini")) {
                rowIndex--;
                t->setRowCount(rowIndex+1);
                subIt++;
                continue;
            }
            if(key.endsWith("lnk")) {
                key.remove(key.length()-4,4);
            }
            t->setItem(rowIndex, 1, new QTableWidgetItem(key));
            setTableItem(t,rowIndex,imagePath,"");
            //t->setItem(rowIndex, 4, new QTableWidgetItem(imagePath));
            subIt++;
        }
        fIt++;

    }



}
void initTableDll(QTableWidget* t, HKEY rootKey, QString path){
    map<string, string> regMap = read_reg(rootKey, path.toLocal8Bit());

    map<string, string>::iterator regIt;
    int rowIndex = t->rowCount();
    QString key, imagePath;
    regIt = regMap.begin();
    if (regMap.empty()) {
        //std::cout<<path.toStdString()<<"empty"<<'\n';
        return;
    }
    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);
    while(regIt != regMap.end()) {
        rowIndex++;
        t->setRowCount(rowIndex+1);
        key = QString::fromStdString(regIt->first);
        imagePath = "C:\\Windows\\system32\\" + QString::fromStdString(regIt->second);
        QTableWidgetItem* keyItem = new QTableWidgetItem(key);
        t->setItem(rowIndex, 1, keyItem);
        RepairString(&imagePath);
        setTableItem(t,rowIndex,imagePath,"");
        regIt++;
    }
}

void initTableActiveX(QTableWidget* t, HKEY rootKey, QString path) {
    map<int, string> subkeyMap;
    map<char*,char*> regMap;
    QString key, imagePath;
    int rowIndex = t->rowCount();

    subkeyMap = read_subkey(rootKey,path.toLocal8Bit());

    string subPath;
    string tmpPath;
    string m_key,m_path;
    map<int, string>::iterator subIt = subkeyMap.begin();

    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);

    while (subIt != subkeyMap.end()) {

        subPath = subIt->second;
        tmpPath = path.toStdString() +"\\"+subPath;
        LPCWSTR newSubKey = (LPCWSTR)char2TCHAR(tmpPath.c_str());
        m_key = read_name(rootKey, newSubKey);
        if(!m_key.empty()) {
            m_path = readStubPath(rootKey, newSubKey);
            if(!m_path.empty()){
                rowIndex++;
                t->setRowCount(rowIndex+1);
                key = QString::fromStdString(m_key);
                imagePath = QString::fromStdString(m_path);
                t->setItem(rowIndex, 1, new QTableWidgetItem(key));
                RepairString(&imagePath);
                setTableItem(t,rowIndex,imagePath,"");
            }
        }

        subIt++;
        delete [] newSubKey;
        newSubKey = NULL;

    }
}
void initTableService(QTableWidget* t,HKEY rootKey, QString path ) {
    map<int, string> subkeyMap;
    map<char*,char*> regMap;
    QString key, imagePath;
    int rowIndex = t->rowCount();

    subkeyMap = read_subkey(rootKey,path.toLocal8Bit());

    string subPath;
    string tmpPath;
    DWORD type, start;
    map<int, string>::iterator subIt = subkeyMap.begin();

    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);

    while (subIt != subkeyMap.end()) {

        subPath = subIt->second;
        key = QString::fromStdString(subIt->second);
        tmpPath = path.toStdString() +"\\"+subPath;
        LPCWSTR newSubKey = (LPCWSTR)char2TCHAR(tmpPath.c_str());
        string m_description, m_imagepath;
        type = read_type(rootKey,newSubKey);
        if (type>=16 && type<100000 ) {
            start = read_start(rootKey,newSubKey);
            if (start == 0 ||start == 1 ||start == 2) {
                rowIndex++;
                t->setRowCount(rowIndex+1);
                m_description = read_description(rootKey,newSubKey);
                m_imagepath = read_imagepath(rootKey,newSubKey);
                t->setItem(rowIndex, 1, new QTableWidgetItem(key));
                QString imagePath = QString::fromStdString(m_imagepath);
                QString description = QString::fromStdString(m_description);
                RepairString(&imagePath);
                if (!imagePath.isEmpty()) {
                    setTableItem(t,rowIndex,imagePath,description);
                }
            }


        }
        subIt++;
        delete [] newSubKey;
        newSubKey = NULL;

    }

}

void initTableDriver(QTableWidget* t,HKEY rootKey, QString path){
    map<int, string> subkeyMap;
    QString key;
    int rowIndex = t->rowCount();

    subkeyMap = read_subkey(rootKey,path.toLocal8Bit());

    string subPath;
    string tmpPath;
    DWORD type,start;
    map<int, string>::iterator subIt = subkeyMap.begin();

    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);

    while (subIt != subkeyMap.end()) {

        subPath = subIt->second;
        key = QString::fromStdString(subIt->second);
        tmpPath = path.toStdString() +"\\"+subPath;
        LPCWSTR newSubKey = (LPCWSTR)char2TCHAR(tmpPath.c_str());
        string m_description, m_imagepath;
        type = read_type(rootKey,newSubKey);
        if (type<16 ) {
            start = read_start(rootKey,newSubKey);
            if (start == 0 ||start == 1 ||start == 2) {
                rowIndex++;
                t->setRowCount(rowIndex+1);
                m_description = read_description(rootKey,newSubKey);
                m_imagepath = read_imagepath(rootKey,newSubKey);
                t->setItem(rowIndex, 1, new QTableWidgetItem(key));
                QString imagePath = QString::fromStdString(m_imagepath);
                QString description = QString::fromStdString(m_description);
                RepairString(&imagePath);
                if (!imagePath.isEmpty()) {
                    setTableItem(t,rowIndex,imagePath,description);
                }

            }

        }
        subIt++;
        delete [] newSubKey;
        newSubKey = NULL;
    }

}

void initTableTask(QTableWidget* t) {
    map<string, string> taskMap;
    QString key, imagePath;
    int rowIndex = t->rowCount();
    drawHeader(t, rowIndex, "Shedule Task");
    entryTask(&taskMap);
    map<string,string>::iterator taskIt = taskMap.begin();
    while(taskIt != taskMap.end()) {
        rowIndex++;
        t->setRowCount(rowIndex+1);
        string m_key = taskIt->first;
        string m_imagePath = taskIt->second;
        key = QString::fromStdString(m_key);
        t->setItem(rowIndex, 1, new QTableWidgetItem(key));
        imagePath = QString::fromStdString(m_imagePath);
        RepairString(&imagePath);
        setTableItem(t,rowIndex,imagePath,"");
        taskIt++;
    }
}


void initTablePara(QTableWidget* t){
    QStringList headerlist = {"icon","entry","description","publisher","path","time"};
    t->setHorizontalHeaderLabels(headerlist);
    t->verticalHeader()->setVisible(false);
    t->setColumnWidth(0,40);
    t->setColumnWidth(1,150);
    t->setColumnWidth(2,250);
    t->setColumnWidth(3,250);
    t->setColumnWidth(4,380);
    t->setColumnWidth(5,180);
}
void drawHeader(QTableWidget* t,int rowIndex, QString path){
    t->setRowCount(rowIndex + 1);
    QTableWidgetItem* regItem = new QTableWidgetItem(path);
    regItem->setBackgroundColor(QColor(100, 200, 255));
    t->setItem(rowIndex, 0, regItem);
    t->setSpan(rowIndex, 0, 1, 6);
}


void RepairString(QString *str)
{
    if (str->contains(" /"))
        *str = str->split(" /")[0];
    if (str->contains(" -"))
        *str = str->split(" -")[0];
    if (str->contains("\""))
        *str = str->replace("\"", "");
    if (str->contains("\\??\\"))
        *str = str->replace("\\??\\", "");
    if (str->contains("%windir%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("%windir%", 0, Qt::CaseInsensitive), 8, "C:\\Windows");
    if (str->contains("@%systemroot%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("@%systemroot%", 0, Qt::CaseInsensitive), 13, "C:\\Windows");
    if (str->contains("%systemroot%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("%systemroot%", 0, Qt::CaseInsensitive), 12, "C:\\Windows");
    if (str->contains("\\SystemRoot", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("\\SystemRoot", 0, Qt::CaseInsensitive), 11, "C:\\Windows");
    if (str->startsWith("system32", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("system32", 0, Qt::CaseInsensitive), 8, "C:\\Windows\\System32");
    if (str->contains("%ProgramData%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("%ProgramData%", 0, Qt::CaseInsensitive), 13, "C:\\ProgramData");
    if (str->contains("%ProgramFiles(x86)%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("%ProgramFiles(x86)%", 0, Qt::CaseInsensitive), 19, "C:\\Program Files (x86)");
    if (str->contains("%ProgramFiles%", Qt::CaseInsensitive))
        *str = str->replace(str->indexOf("%ProgramFiles%", 0, Qt::CaseInsensitive), 14, "C:\\Program Files");
}

