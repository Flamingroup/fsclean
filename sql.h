#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"
#include <QSqlQueryModel>
#include <QThread>
#include <QMutex>

class Sql {
    //mapute0682524138
private :
		QSqlDatabase db;
		static Sql* _instance;
		Sql(path* p=new path("fsclean.db"));
        QMutex mutex;
public:
		static Sql* getInstance();
		char sqlInsert(const Fichier &);
		void sqlDelDeletedFiles();
		void sqlRaz();
		bool sqlUpdate(Fichier);
        void Affiche();
        bool sqlCreateMD5();
		bool sqlDelete(string chemin);
        bool sqlSetMd5(Fichier &f);
		QSqlQueryModel *sqlSelect(string requete="SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers WHERE(MD5 IN(SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1)) OR (filenametrime IN (SELECT filenametrime FROM Fichiers WHERE 1 GROUP BY filenametrime HAVING COUNT(filenametrime)>1)) ORDER BY MD5,chemin");
        //QSqlQueryModel *sqlSelect(string requete="SELECT * FROM Fichiers WHERE MD5 IN (SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1) ORDER BY MD5");
};

#endif // SQL_H
