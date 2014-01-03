#include "mainwindow.h"
#include "ui_interface.h"
#include "parcours.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scanButton_clicked()
{
    Parcours parc;
    parc.runAll();
}

void MainWindow::displayInStatusBar(const std::string & message) {
    statusBar()->showMessage(message.c_str());
}
