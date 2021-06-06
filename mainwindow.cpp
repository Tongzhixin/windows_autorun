#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "regread.h"
#include "signature.h"
#include "convert.h"
#include "description.h"
#define logon { "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\Run",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce",\
                "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnceEx", \
                "SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run"}
#define services "System\\CurrentControlSet\\Services"
#define knowndlls "System\\CurrentControlSet\\Control\\Session Manager\\KnownDlls"
void drawHeader(QTableWidget *table, int rowIndex, QString path);
void initTablePara(QTableWidget* t);
void initTableLogon(QTableWidget* t, HKEY rootKey, QString path);
void initTableService(QTableWidget* t);
void initTableTask(QTableWidget* t);
void initTableDll(QTableWidget* t);
void initTableDriver(QTableWidget* t);
void setTableItem(QTableWidget* t, int rowIndex, QString imagePath, QString description);
void RepairString(QString *str);
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

    foreach(QString path, QStringList logon)
    {
        initTableLogon(ui->tableWidget_logon, HKLM, path);
        initTableLogon(ui->tableWidget_logon, HKCU, path);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}
void test(){
    HKEY rootKey = HKLM;
    QString path = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    map<char*, char*> regMap = read_reg(rootKey, path.toLocal8Bit());

    map<char*, char*>::iterator regIt = regMap.begin();
    QString key, imagePath;
    while(regIt != regMap.end()) {
        key = QString(regIt->first);
        imagePath = QString(regIt->second);
        qDebug()<<key<<":"<<imagePath;
        regIt++;
    }

}
void setTableItem(QTableWidget* t, int rowIndex, QString imagePath, QString description) {
    t->setRowHeight(rowIndex, 30);
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

    QString *buf_pub = new QString;
    GetSignaturePublisher(char2TCHAR(QString2char(imagePath)), buf_pub);
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
}
void initTableLogon(QTableWidget* t, HKEY rootKey, QString path){
    map<char*, char*> regMap = read_reg(rootKey, path.toLocal8Bit());

    map<char*, char*>::iterator regIt;
    int rowIndex = t->rowCount();
    QString key, imagePath;
    regIt = regMap.begin();
    if (regMap.empty()) {
        std::cout<<path.toStdString()<<"empty"<<'\n';
        return;
    }
    QString HKheader = (rootKey==HKLM) ? "HKLM\\" : "HKCU\\";
    drawHeader(t, rowIndex, HKheader+path);
    while(regIt != regMap.end()) {
        rowIndex++;
        t->setRowCount(rowIndex+1);
        key = QString(regIt->first);
        imagePath = QString(regIt->second);
        QTableWidgetItem* keyItem = new QTableWidgetItem(key);
        t->setItem(rowIndex, 1, keyItem);
        RepairString(&imagePath);
        setTableItem(t,rowIndex,imagePath,"");
        regIt++;
    }
}

void initTablePara(QTableWidget* t){
    t->setColumnWidth(0,20);
    t->setColumnWidth(1,145);
    t->setColumnWidth(2,210);
    t->setColumnWidth(3,200);
    t->setColumnWidth(4,420);
    t->setColumnWidth(5,200);
}
void drawHeader(QTableWidget* t,int rowIndex, QString path){
    t->setRowCount(rowIndex + 1);
    QTableWidgetItem* regItem = new QTableWidgetItem(path);
    regItem->setBackgroundColor(QColor(209, 209, 255));
    t->setItem(rowIndex, 0, regItem);
    t->setSpan(rowIndex, 0, 1, 6);
}

void MainWindow::on_logon_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_service_clicked(){
ui->stackedWidget->setCurrentIndex(1);
}
void MainWindow::on_driver_clicked(){
ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_sheduletask_clicked(){
ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_knowndll_clicked(){
ui->stackedWidget->setCurrentIndex(4);
}
void MainWindow::on_tableWidget_logon_itemClicked(QTableWidgetItem *item){

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

