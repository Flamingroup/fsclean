#include "mainwindow.h"
#include "ui_interface.h"
#include "parcours.h"
#include "sql.h"
#include <QString>
#include <QMessageBox>
#include <QFileDialog>
#include <QSqlQueryModel>
#include <list>
#include <QModelIndex>
#include <QModelIndexList>
#include "thread.h"
#include <sstream>
#include "parcours.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    scan = new Thread;
    Parcours p;
    ostringstream os;
    ui->setupUi(this);
    cout<<"Initialisation GUI"<<endl;
    remplirWL(p);
    remplirBL(p);
    on_Buttonrafraichir_clicked();   
    os<<p.getNbApprox();
    string str = os.str();
    str+=" fichier(s) en base de données.";
    displayInStatusBar(str);
    cout<<"FIN Initialisation GUI\n"<<endl;

}

MainWindow::~MainWindow()
{
    delete scan;
    delete ui;
}

void MainWindow::on_scanButton_clicked()
{
    //todo ici inserer thread
    cout<<"lancement thread.."<<endl;
    scan->start();
    if(scan->isRunning()){
        cout<<"thread running dans scan_button, l'autre flux d'exec continue"<<endl;
        displayInStatusBar("Scan en cours...");
    }
    //ou mettre isFinished? il n'y a qu'un thread, comment reset son etat?
    //if(scan->isFinished()){
    //    cout<<"le thread a terminé."<<endl;
    //    displayInStatusBar("Scan terminé.");
    //}
    //Parcours parc;
    //parc.runAll();
    //on_Buttonrafraichir_clicked();
}

void MainWindow::displayInStatusBar(const std::string & message) {
    statusBar()->showMessage(message.c_str());
}

void MainWindow::remplirWL(Parcours &remplir)
{
    map<string, path*>::iterator it = remplir.listeblanche.begin();
    map<string, path*>::iterator fin = remplir.listeblanche.end();
    ui->listwhiteList->clear();//on nettoie avant de remplir
    for(;it!=fin;it++){
        ui->listwhiteList->addItem(QString::fromStdString((*it).first));
    }
}

void MainWindow::remplirBL(Parcours &remplir)
{
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
        remplirWL(p);
    }
    //todo : gérer suppression multiple

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
        remplirBL(p);
    }
    //todo : gérer suppression multiple

}


void MainWindow::on_actionReinitialiser_param_defaut_triggered()
{
    Parcours p;
    p.resetFicCfg();
    remplirBL(p);
    remplirWL(p);
    on_Buttonrafraichir_clicked();
}

void MainWindow::on_WLplusButton_clicked()
{
    //todo : popup d'entrée d'un chemin, test du chemin et ajout
    //si existe : ajout
    //            regen config
    //pe plus tard : test si scan en cours avant de modif config
    QString inclusion = QFileDialog::getExistingDirectory(this, "Ajouter au scan", "/", QFileDialog::ShowDirsOnly );
    if(inclusion.length() > 0){
        Parcours p;
        p.addToWL(inclusion.toStdString());
        p.regenerateFicCfg();
        remplirWL(p);
        inclusion+=" est maintenant dans la liste blanche.";
        QMessageBox::information(this, "Ajout dans la white list",inclusion);
    }

}

void MainWindow::on_plusBLButton_clicked()
{
    //todo : popup d'entrée d'un chemin
    //si existe : ajout
    //          regen config
    QString exclusion = QFileDialog::getExistingDirectory(this, "Bannir des scans", "/", QFileDialog::ShowDirsOnly );
    if(exclusion.length() > 0){
        Parcours p;
        p.addToBL(exclusion.toStdString());
        p.regenerateFicCfg();
        remplirBL(p);
        exclusion+=" est maintenant dans la liste noire.";
        QMessageBox::information(this, "Ajout dans la black list",exclusion);
    }
}

void MainWindow::on_actionA_propos_triggered()
{
    popup = new About(this);
    popup->show();
}


void MainWindow::on_Buttonrafraichir_clicked()
{
    cout<<"rafraichir_button"<<endl;
    static bool prem = true;
    Sql* mabase=Sql::getInstance();
	QSqlQueryModel *reponse = mabase->sqlSelect();
    if(! prem){
        ui->TableAffichageDoublons->selectAll();
        QItemSelectionModel * table = ui->TableAffichageDoublons->selectionModel();
        QModelIndexList indexes = table->selectedIndexes();
        for(QModelIndex i :indexes)
            ui->TableAffichageDoublons->showRow(i.row());
    }
    ui->TableAffichageDoublons->setModel(reponse);
    prem = false;
}

void MainWindow::on_Buttonsupprimer_clicked()
{
    //todo : gérer suppression multiple
}

void MainWindow::on_Buttonmasquer_clicked()
{
    //done : gérer masquage multiple
    QItemSelectionModel * lignes = ui->TableAffichageDoublons->selectionModel();
    QModelIndexList indexes = lignes->selectedIndexes();
    for(QModelIndex i :indexes)
        ui->TableAffichageDoublons->hideRow(i.row());
    //done : gérer dans rafraichir l'affichage de toutes les lignes
}
