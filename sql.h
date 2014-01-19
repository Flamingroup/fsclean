#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"
#include <QSqlQueryModel>
#include <QThread>
#include <QMutex>

struct SelectDoublon {
	static string MD5 = "SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers WHERE(MD5 IN(SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1))";
	static string filenametrime = "SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers filenametrime IN (SELECT filenametrime FROM Fichiers WHERE 1 GROUP BY filenametrime HAVING COUNT(filenametrime)>1) ORDER BY MD5,chemin";
	static string doublons = "SELECT chemin FROM Dossiers WHERE isdoublon = 1";
};

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
		bool isDossierDoublon(const string& chemin);
		bool sqlSetDossierDoublons();
		QSqlQueryModel* sqlSelectDoublonsMD5();
		QSqlQueryModel* sqlSelectDoublonsFilenametrime();
		QSqlQueryModel* sqlSelectDoublonsDossiers();
};

#endif // SQL_H
