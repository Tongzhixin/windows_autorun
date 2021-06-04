#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "regread.h"
#include "signature.h"
#include "convert.h"
#include "description.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

