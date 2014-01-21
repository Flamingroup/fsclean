#include <iostream>
#include <boost/filesystem.hpp>
#include "parcours.h"
#include "sql.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <QApplication>
#include <QtGui>
#include "mainwindow.h"

using namespace boost::filesystem;
using namespace std;

int main(int argc, char *argv[])
{  
	if (argc == 1){
		cout << "Utilisation : \n\t -gui Lance le programme avec l'interface graphique" << endl
			 << "\t -text : Affiche les doublons en base de donnée" << endl
			 << "\t -scan pour lancer un scan (peut être placé dans le cron weekly)" << endl
			 << endl;
	}
	if (argc > 1 && string(argv[1]) == "-gui") {
		QApplication app(argc,argv);
		MainWindow fenetre;
		fenetre.show();
		return app.exec();
	}
	else if (argc > 1 && string(argv[1]) == "-text") {
		Sql* s = Sql::getInstance();
		s->Affiche("MD5");
		s->Affiche("nom");
		s->Affiche("dossier");
	}
	else if (argc > 1 && string(argv[1]) == "-scan") {
		cout << "Début scan..." << endl;
		Parcours* parc=Parcours::getInstance();
		parc->regenerateFicCfg(1);
		parc->runAll();
		cout << "Fin scan." << endl;
	}
}
