#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <windows.h>
#include <QSettings>
#include <QFileInfo>
#include <QPushButton>
#include <QFileIconProvider>
#include <QDateTime>
#include <QProcess>
#include <Qt>
#include <QTableWidgetItem>
#include <QTextCodec>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void on_logon_clicked();
    void on_service_clicked();
    void on_driver_clicked();
    void on_sheduletask_clicked();
    void on_knowndll_clicked();

private:
    Ui::MainWindow *ui;


};
void test();

#endif // MAINWINDOW_H
