#include "mainwindow.h"
#include "ui_interface.h"
#include "parcours.h"
#include "sql.h"
#include <QString>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    remplirWL();
    remplirBL();
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

void MainWindow::on_ButtonOKinexclure_clicked()
{
    //todo: ici verifier les button radio puis les inclure/exclure puis le line edit
    //verifier ensuite si le chemin entré existe
    //si oui : si WL/BL coché si inclure/exclure coché ajouter/enlever (plus chaud) de config
    //sinon : rien faire
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
    if(ui->listwhiteList->count()){
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
    if(ui->listblackList->count()){
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
