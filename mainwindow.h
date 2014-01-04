#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "about.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void displayInStatusBar(const std::string & message);
private slots:
    void on_scanButton_clicked();

    void remplirWL();
    void remplirBL();

    void on_lessWLButton_clicked();

    void on_lessBLButton_clicked();
    void on_actionReinitialiser_param_defaut_triggered();

    void on_WLplusButton_clicked();

    void on_plusBLButton_clicked();

    void on_actionA_propos_triggered();

private:
    Ui::MainWindow *ui;
    About *popup;
};

#endif // MAINWINDOW_H