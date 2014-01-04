#include "mainwindow.h"
#include "ui_interface.h"
#include "parcours.h"
#include "sql.h"
#include <QString>
#include <QMessageBox>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    remplirWL();
    remplirBL();
    displayInStatusBar("Ici, prochainement plein d'infos sur le scan.");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_scanButton_clicked()
{
    Parcours parc;
    parc.runAll();
    Sql* mabase=Sql::getInstance();
    mabase->Affiche();
}

void MainWindow::displayInStatusBar(const std::string & message) {
    statusBar()->showMessage(message.c_str());
}

void MainWindow::remplirWL()
{
    Parcours remplir;
    map<string, path*>::iterator it = remplir.listeblanche.begin();
    map<string, path*>::iterator fin = remplir.listeblanche.end();
    ui->listwhiteList->clear();//on nettoie avant de remplir
    for(;it!=fin;it++){
        ui->listwhiteList->addItem(QString::fromStdString((*it).first));
    }
}

void MainWindow::remplirBL()
{
    Parcours remplir;
    map<string, path*>::iterator it = remplir.listenoire.begin();
    map<string, path*>::iterator fin = remplir.listenoire.end();
    ui->listblackList->clear();//on nettoie avant de remplir
    for(;it!=fin;it++){
        ui->listblackList->addItem(QString::fromStdString((*it).first));
    }
}

void MainWindow::on_lessWLButton_clicked()
{
    //enlever de la whitelist gui, de la map et de la vraie dans config
    //pour modifier la config : si map modifiée, config effacée et nouvelle config vaut listeB+N
    if(ui->listwhiteList->count() && ui->listwhiteList->selectedItems().count()){
        QString s = ui->listwhiteList->currentItem()->text();
        std::cout<<"lessWLButton enleve = "<<s.toStdString()<<endl;
        Parcours p;
        p.rmvFromWL(s.toStdString());
        p.regenerateFicCfg();
        remplirWL();
    }
    //todo : gérer le plantage si aucun élément selectionné(par défaut au début)
}

void MainWindow::on_lessBLButton_clicked()
{
    //enlever de la blacklist gui, de la map et de la vraie dans config
    //pour modifier la config : si map modifiée, config effacée et nouvelle config vaut listeB+N
    if(ui->listblackList->count() && ui->listblackList->selectedItems().count()){
        QString s = ui->listblackList->currentItem()->text();
        std::cout<<"lessWLButton enleve = "<<s.toStdString()<<endl;
        Parcours p;
        p.rmvFromBL(s.toStdString());
        p.regenerateFicCfg();
        remplirBL();
    }
}


void MainWindow::on_actionReinitialiser_param_defaut_triggered()
{
    Parcours p;
    p.resetFicCfg();
    remplirBL();
    remplirWL();
}

void MainWindow::on_WLplusButton_clicked()
{
    //todo : popup d'entrée d'un chemin, test du chemin et ajout
    //si existe : ajout
    //            regen config
    //pe plus tard : test si scan en cours avant de modif config
    QString inclusion = QFileDialog::getExistingDirectory(this, "Ajouter au scan", "/", QFileDialog::ShowDirsOnly );
    if(inclusion.length() > 0)
        QMessageBox::information(this, "Ajout dans la white list",inclusion);
    cout<<"inclusion="<<inclusion.toStdString()<<endl;
    Parcours p;
    p.addToWL(inclusion.toStdString());
    p.regenerateFicCfg();
    remplirWL();
}

void MainWindow::on_plusBLButton_clicked()
{
    //todo : popup d'entrée d'un chemin
    //si existe : ajout
    //          regen config
    QString exclusion = QFileDialog::getExistingDirectory(this, "Bannir des scans", "/", QFileDialog::ShowDirsOnly );
    if(exclusion.length() > 0)
        QMessageBox::information(this, "Ajout dans la black list",exclusion);
    cout<<"exclusion="<<exclusion.toStdString()<<endl;
    Parcours p;
    p.addToBL(exclusion.toStdString());
    p.regenerateFicCfg();
    remplirBL();
}

void MainWindow::on_actionA_propos_triggered()
{
    popup = new About(this);
    popup->show();
}
