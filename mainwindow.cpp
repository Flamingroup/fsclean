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
    timer->start(100);
    cout<<"Initialisation GUI"<<endl;
	remplirWL();
	remplirBL();
	displayNbElemBDDInStatusBar();
	ui->progressBar->setValue(Parcours::AVANCE);
	on_Buttonrafraichir_clicked();
	ui->TableAffichageDoublons->resizeColumnsToContents();
    cout<<"FIN Initialisation GUI\n"<<endl;
}

MainWindow::~MainWindow()
{
	if (scan != NULL){
		delete scan;
	}
    delete ui;
	delete timer;
}

void MainWindow::on_scanButton_clicked()
{
    //todo ici inserer thread
    cout<<"lancement thread.."<<endl;
	if (scan == NULL){
		scan=new Thread;
	}//normalement else inutile
	else {scan->quit(); cout<<"else"<<endl;}
    setLEDRed();
	scan->start();
    if(scan->isRunning()){
        displayInStatusBar("Scan en cours...");
    }
    //ou mettre isFinished? il n'y a qu'un thread, comment reset son etat?
	if (!scan->isRunning()){
		delete scan;
		scan=NULL;
	}
}

void MainWindow::displayInStatusBar(const std::string & message) {
	statusBar()->showMessage(message.c_str());
}

void MainWindow::displayNbElemBDDInStatusBar()
{
	Parcours *p=Parcours::getInstance();
	ostringstream os;
	os<<p->getNbApprox();
	string str = os.str();
	str+=" fichier(s) en base de donnees.";
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
    displayInStatusBar("Scan termine.");
	setLEDGreen();
	displayNbElemBDDInStatusBar();
	on_Buttonrafraichir_clicked();
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
		std::cout<<"lessWLButton enleve = "<<s.toStdString()<<endl;
		Parcours *p=Parcours::getInstance();
		p->rmvFromWL(s.toStdString());
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
		std::cout<<"lessWLButton enleve = "<<s.toStdString()<<endl;
		Parcours *p=Parcours::getInstance();
		p->rmvFromBL(s.toStdString());
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
    //todo : popup d'entrée d'un chemin, test du chemin et ajout
    //si existe : ajout
    //            regen config
    //pe plus tard : test si scan en cours avant de modif config
    QString inclusion = QFileDialog::getExistingDirectory(this, "Ajouter au scan", "/", QFileDialog::ShowDirsOnly );
	if(inclusion.length() > 0){
		Parcours *p=Parcours::getInstance();
		p->addToWL(inclusion.toStdString());
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
	cout<<"rafraichir_button"<<endl;
	if (scan->isRunning())
		return;
	static bool prem = true;
	if(! prem){
		ui->TableAffichageDoublons->selectAll();
		QItemSelectionModel * table = ui->TableAffichageDoublons->selectionModel();
		QModelIndexList indexes = table->selectedIndexes();
		for(QModelIndex i :indexes)
			ui->TableAffichageDoublons->showRow(i.row());
	}
	if (!scan->isRunning()){
		Sql* mabase=Sql::getInstance();
		QSqlQueryModel *reponse = mabase->sqlSelect();
		ui->TableAffichageDoublons->setModel(reponse);
	}
    prem = false;
    cout<<"fin rafraichir"<<endl;
}

void MainWindow::on_Buttonsupprimer_clicked()
{
	cout<<"debut"<<endl;
	//todo : gérer suppression (multiple)
	try {
		QItemSelectionModel * lignes = ui->TableAffichageDoublons->selectionModel();
		QModelIndexList indexes = lignes->selectedIndexes();
		for(QModelIndex i :indexes){
			cout<<"avant" << endl;
			QStandardItemModel item(1,1,ui->TableAffichageDoublons);
			if(item.insertRow(1,i)) cout<<"ok"<<endl;
			else cout<<"wrong"<<endl;
			cout<<"apres"<<endl;
			//string s =item.item(i.row())->data().toString().toStdString();
			cout<<"remove : "<<endl;
			//cout << s << endl;
			//bool valid = file.exists();
			//bool valid = file.remove();
			item.removeRow(i.row());
			//plus virer de bdd DELETE(string)
		}
	}
	catch (...){

	}
}

void MainWindow::on_Buttonmasquer_clicked()
{
    QItemSelectionModel * lignes = ui->TableAffichageDoublons->selectionModel();
    QModelIndexList indexes = lignes->selectedIndexes();
    for(QModelIndex i :indexes)
        ui->TableAffichageDoublons->hideRow(i.row());
}
