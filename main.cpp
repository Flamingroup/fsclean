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
<<<<<<< HEAD
    ///GUI
    QApplication app(argc,argv);
    MainWindow fenetre;
    fenetre.show();
    return app.exec();

    //Parcours* parc=Parcours::getInstance();
    //parc->runAll();
=======
    ///soon
	QApplication app(argc,argv);
	MainWindow fenetre;
	fenetre.show();
	return app.exec();

//	Parcours* parc=Parcours::getInstance();
//	parc->runAll();
>>>>>>> 0bb598a5c70efc248534b581eeb45200fae6adbc
}
