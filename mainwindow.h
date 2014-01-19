#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "about.h"
#include <QMainWindow>
#include "thread.h"
#include "parcours.h"

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
	void displayNbElemBDDInStatusBar();
    Thread * scan;
	QTimer *timer;
public slots:
	void setProgress();
    void FinScan();
    void bloquerSuppr(int index);
private slots:
    void setLEDGreen();
    void setLEDRed();
    void on_scanButton_clicked();

	void remplirWL();
	void remplirBL();

    void on_lessWLButton_clicked();

    void on_lessBLButton_clicked();

    void on_actionReinitialiser_param_defaut_triggered();

    void on_WLplusButton_clicked();

    void on_plusBLButton_clicked();

    void on_actionA_propos_triggered();


	void on_Buttonrafraichir_clicked();

    void on_Buttonsupprimer_clicked();

    void on_Buttonmasquer_clicked();

    void on_quitButton_clicked();

private:
    Ui::MainWindow *ui;
	About *popup;
};

#endif // MAINWINDOW_H
