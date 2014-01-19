#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"
#include <QSqlQueryModel>
#include <QThread>
#include <QMutex>

class Sql {
private :
		QSqlDatabase db;
		static Sql* _instance;
		Sql(path* p=new path("fsclean.db"));
        QMutex mutex;
public:
		static Sql* getInstance();
		char sqlInsert(const Fichier &);
		char sqlInsertDossier(const string& str);
		void sqlDelDeletedFiles();
		void sqlRaz();
		bool sqlUpdate(Fichier);
        void Affiche();
        bool sqlCreateMD5();
		bool sqlDelete(string chemin);
		bool sqlSetMd5(Fichier &f);
		bool sqlSetDossierDoublons();
		QSqlQueryModel* sqlSelectDoublonsMD5();
		QSqlQueryModel* sqlSelectDoublonsFilenametrime();
		QSqlQueryModel* sqlSelectDossiersDoublons();
};

#endif // SQL_H
