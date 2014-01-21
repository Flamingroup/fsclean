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
#include <QTimer>
#include <QStandardItemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	scan = new Thread;
    ui->setupUi(this);
    ui->LED->setScaledContents(true);
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(setProgress()));
    connect(scan,SIGNAL(scanFinished()),this,SLOT(FinScan()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(bloquerSuppr(int)));
    timer->start(100);
	remplirWL();
	remplirBL();
	displayNbElemBDDInStatusBar();
	ui->progressBar->setValue(Parcours::AVANCE);
    ui->tabWidget->setCurrentIndex(1);
    on_Buttonrafraichir_clicked();
    ui->tabWidget->setCurrentIndex(2);
    on_Buttonrafraichir_clicked();
    ui->tabWidget->setCurrentIndex(0);
    on_Buttonrafraichir_clicked();
    ui->tableDoublonsFicS->resizeColumnsToContents();
    ui->tableDoublonsFicP->resizeColumnsToContents();
    ui->tableDoublonsD->resizeColumnsToContents();
    ui->quitButton->setDisabled(true);
    cout<<"GUI [ok]"<<endl;
}

MainWindow::~MainWindow()
{
    delete scan;
    delete ui;
	delete timer;
}

void MainWindow::on_scanButton_clicked()
{
    //cout<<"lancement thread.."<<endl;
    if(!scan->isRunning()){
        Parcours *p = Parcours::getInstance();
        if(p->listeblanche.empty()){
            p->addToWL("/");
            QMessageBox::warning(this, "Avertissement", "La liste blanche est vide, la racine sera scannée.");
        }
        ui->plusBLButton->setDisabled(true);
        ui->WLplusButton->setDisabled(true);
        ui->lessBLButton->setDisabled(true);
        ui->lessWLButton->setDisabled(true);
        ui->scanButton->setDisabled(true);
        ui->quitButton->setDisabled(false);
        setLEDRed();
        scan->start();
        cout << "Scan en cours..." << endl;
        displayInStatusBar("Scan en cours...");
    }
}

void MainWindow::displayInStatusBar(const std::string & message) {
	statusBar()->showMessage(message.c_str());
}

void MainWindow::displayNbElemBDDInStatusBar()
{
	Parcours *p=Parcours::getInstance();
	ostringstream os;
    //p->countApprox();
    os<<p->getNbApprox();
	string str = os.str();
    str+=" fichiers potentiels dans la zone de scan.";
	displayInStatusBar(str);
}

void MainWindow::setProgress()
{
	//normalement la valeur est toujours entre 0 et 100
	if(Parcours::AVANCE > 100)
		Parcours::AVANCE = 100;
	else if(Parcours::AVANCE < 0)
		Parcours::AVANCE = 0;
    ui->progressBar->setValue(Parcours::AVANCE);
    if(Parcours::AVANCE==100 && scan->isRunning())
        displayInStatusBar("indexation terminee. Mise a jour de la base en cours...");
}

void MainWindow::FinScan()
{
    ui->plusBLButton->setDisabled(false);
    ui->WLplusButton->setDisabled(false);
    ui->lessBLButton->setDisabled(false);
    ui->lessWLButton->setDisabled(false);
    ui->scanButton->setDisabled(false);
	setLEDGreen();
	displayNbElemBDDInStatusBar();
    on_Buttonrafraichir_clicked();
    ui->tabWidget->setCurrentIndex(1);
    on_Buttonrafraichir_clicked();
    ui->tabWidget->setCurrentIndex(2);
    on_Buttonrafraichir_clicked();
    ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::bloquerSuppr(int index)
{
    if(index == 2)
        ui->Buttonsupprimer->setDisabled(true);
    else ui->Buttonsupprimer->setEnabled(true);
}

void MainWindow::setLEDGreen()
{
    QPixmap green("./green.png") ;
    ui->LED->setPixmap(green);
}

void MainWindow::setLEDRed()
{
    QPixmap red("./red.png");
    ui->LED->setPixmap(red);
}

void MainWindow::remplirWL()
{
	Parcours *remplir=Parcours::getInstance();
	map<string, path*>::iterator it = remplir->listeblanche.begin();
	map<string, path*>::iterator fin = remplir->listeblanche.end();
    ui->listwhiteList->clear();//on nettoie avant de remplir
    for(;it!=fin;it++){
        ui->listwhiteList->addItem(QString::fromStdString((*it).first));
    }
}

void MainWindow::remplirBL()
{
	Parcours *remplir=Parcours::getInstance();
	map<string, path*>::iterator it = remplir->listenoire.begin();
	map<string, path*>::iterator fin = remplir->listenoire.end();
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
        cout<<"Oté de la liste blanche : "<<s.toStdString()<<endl;
		Parcours *p=Parcours::getInstance();
		p->rmvFromWL(s.toStdString());
        p->countApprox();
		p->regenerateFicCfg();
		remplirWL();
		displayNbElemBDDInStatusBar();
    }
}

void MainWindow::on_lessBLButton_clicked()
{
    //enlever de la blacklist gui, de la map et de la vraie dans config
    //pour modifier la config : si map modifiée, config effacée et nouvelle config vaut listeB+N
    if(ui->listblackList->count() && ui->listblackList->selectedItems().count()){
        QString s = ui->listblackList->currentItem()->text();
        cout<<"Ajouté à la liste blanche : "<<s.toStdString()<<endl;
		Parcours *p=Parcours::getInstance();
		p->rmvFromBL(s.toStdString());
        p->countApprox();
		p->regenerateFicCfg();
		remplirBL();
		displayNbElemBDDInStatusBar();
    }
}


void MainWindow::on_actionReinitialiser_param_defaut_triggered()
{
	Parcours *p=Parcours::getInstance();
	p->resetFicCfg();
	remplirBL();
	remplirWL();
    on_Buttonrafraichir_clicked();
}

void MainWindow::on_WLplusButton_clicked()
{
    QString inclusion = QFileDialog::getExistingDirectory(this, "Ajouter au scan", "/", QFileDialog::ShowDirsOnly );
	if(inclusion.length() > 0){
		Parcours *p=Parcours::getInstance();
		p->addToWL(inclusion.toStdString());
        p->countApprox();
		p->regenerateFicCfg();
		remplirWL();
        inclusion+=" est maintenant dans la liste blanche.";
        QMessageBox::information(this, "Ajout dans la white list",inclusion);
		displayNbElemBDDInStatusBar();
    }

}

void MainWindow::on_plusBLButton_clicked()
{
    //todo : popup d'entrée d'un chemin
    //si existe : ajout
    //          regen config
    QString exclusion = QFileDialog::getExistingDirectory(this, "Bannir des scans", "/", QFileDialog::ShowDirsOnly );
	if(exclusion.length() > 0){
		Parcours *p=Parcours::getInstance();
		p->addToBL(exclusion.toStdString());
        p->countApprox();
		p->regenerateFicCfg();
		remplirBL();
        exclusion+=" est maintenant dans la liste noire.";
        QMessageBox::information(this, "Ajout dans la black list",exclusion);
		displayNbElemBDDInStatusBar();
    }
}

void MainWindow::on_actionA_propos_triggered()
{
    popup = new About(this);
    popup->show();
}


void MainWindow::on_Buttonrafraichir_clicked()
{
    cout <<"Rafraichissement de l'onglet "<<ui->tabWidget->currentIndex()<< "..." <<endl;
    if (scan->isRunning()){
        cout <<"    Vous ne pouvez rafraichir pendant un scan."<<endl;
		return;
    }
    static bool premFS = true;
    static bool premFP = true;
    static bool premDP = true;
    if(! premFS){
        if(ui->tabWidget->currentIndex() == 0){//case 1st tab
            ui->tableDoublonsFicS->selectAll();
            QItemSelectionModel *table = ui->tableDoublonsFicS->selectionModel();
            QModelIndexList indexes = table->selectedIndexes();
            for(QModelIndex i :indexes)
                ui->tableDoublonsFicS->showRow(i.row());
        }
    }
    if(!premFP){
        if(ui->tabWidget->currentIndex() == 1){//case 2nd tab
                ui->tableDoublonsFicP->selectAll();
                QItemSelectionModel * table = ui->tableDoublonsFicP->selectionModel();
                QModelIndexList indexes = table->selectedIndexes();
                for(QModelIndex i :indexes)
                    ui->tableDoublonsFicP->showRow(i.row());
            }
    }
    if(!premDP){
            if(ui->tabWidget->currentIndex() == 2){//case 3rd tab
            ui->tableDoublonsD->selectAll();
            QItemSelectionModel * table = ui->tableDoublonsD->selectionModel();
            QModelIndexList indexes = table->selectedIndexes();
            for(QModelIndex i :indexes)
                ui->tableDoublonsD->showRow(i.row());
            }
    }
    cout <<"    Objets cachés révélés"<<endl;
    //j'ai tenté cette méthode avec des switch : tonne de problemes avec les déclarations
    //de variable dans les if, finalement pas plus simple que des if imbriqués
    //if (!scan->isRunning()){
    Sql* mabase=Sql::getInstance();
    if(ui->tabWidget->currentIndex() == 0){//onglet 0 fichiers surs
        QSqlQueryModel *reponse = mabase->sqlSelectDoublons(mabase->MD5);
        ui->tableDoublonsFicS->setModel(reponse);
        ui->tableDoublonsFicS->resizeColumnsToContents();
        premFS = false;
    }
    else if(ui->tabWidget->currentIndex() == 1){//onglet 1 fichiers potentiels
        QSqlQueryModel *reponse = mabase->sqlSelectDoublons(mabase->filenametrime);
        ui->tableDoublonsFicP->setModel(reponse);
        ui->tableDoublonsFicP->resizeColumnsToContents();
        premFP = false;
    }
    else if(ui->tabWidget->currentIndex() == 2){//onglet 2 dossiers potentiels
        QSqlQueryModel *reponse = mabase->sqlSelectDoublons(mabase->dossier);
        ui->tableDoublonsD->setModel(reponse);
        ui->tableDoublonsD->resizeColumnsToContents();
        premDP = false;
    }
    cout << "    Rafraichissement terminé."<<endl;
}

void MainWindow::on_Buttonsupprimer_clicked()
{
    cout<<"Suppression en cours..."<<endl;
		try {
            QItemSelectionModel *lignes;
            if(ui->tabWidget->currentIndex() == 0)//case : 1st tab
                lignes = ui->tableDoublonsFicS->selectionModel();
            else lignes = ui->tableDoublonsFicP->selectionModel();//case 2nd tab
            QModelIndexList indexes = lignes->selectedIndexes();
			for(QModelIndex i :indexes){
				QString s = i.data(0).toString();
				cout<<"s=" << s.toStdString()<< endl;
				QFile file(s);
				if(file.exists()){
					Sql* mabase = Sql::getInstance();
					mabase->sqlDelete(s.toStdString());
					if(file.remove()){
                        cout <<"    " << s.toStdString() << " a été supprimé du disque." << endl;
					}
				}
                else cout << "    " << s.toStdString() << " Ce fichier n'existe pas." << endl;
            }
        }
		catch (...){
		}
    //à la fin des suppressions, on rafraichit
    cout << "    Fin supression." << endl;
    on_Buttonrafraichir_clicked();
}

void MainWindow::on_Buttonmasquer_clicked()
{   //tab 0 : doublons fichiers, 1 : doublons fichiers potentiels, 2 : doublons dossiers
    int numTab = ui->tabWidget->currentIndex();
    if(numTab == 0 ){
        QItemSelectionModel * lignes = ui->tableDoublonsFicS->selectionModel();
        QModelIndexList indexes = lignes->selectedIndexes();
        for(QModelIndex i :indexes)
            ui->tableDoublonsFicS->hideRow(i.row());
    }
    else  if(numTab == 1){
        QItemSelectionModel * lignes = ui->tableDoublonsFicP->selectionModel();
        QModelIndexList indexes = lignes->selectedIndexes();
        for(QModelIndex i :indexes)
            ui->tableDoublonsFicP->hideRow(i.row());
    }
    else if(numTab == 2){
        QItemSelectionModel * lignes = ui->tableDoublonsD->selectionModel();
        QModelIndexList indexes = lignes->selectedIndexes();
        for(QModelIndex i :indexes)
            ui->tableDoublonsD->hideRow(i.row());
    }
}

void MainWindow::on_quitButton_clicked()
{
    cout << "QuitButton_clicked()..." << endl;
    if(scan->isRunning()){
        ui->quitButton->setDisabled(true);
        cout << "    Arret du scan en cours..." << endl;
        displayInStatusBar("Arret du scan en cours...");
        Parcours::STOP=true;
        //scan->exit(0);
        scan->wait();
        Parcours::STOP=false;
        cout << "    Thread enfin fini..." << endl;
        Parcours *p  = Parcours::getInstance();
        p->countApprox();
        p->regenerateFicCfg();
        ui->scanButton->setDisabled(false);
    }
    cout << "Fin quitBurron_clicked()." << endl;
}
