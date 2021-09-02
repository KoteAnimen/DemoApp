#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QPixmap p(":/img/transparent.png");

    ui->lbl_logo->setPixmap(p.scaled(ui->lbl_logo->width(), ui->lbl_logo->height(), Qt::KeepAspectRatio));
}

MainWindow::~MainWindow()
{
    delete ui;
}

