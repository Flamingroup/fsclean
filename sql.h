#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"
#include <QSqlQueryModel>

class Sql {
private :
		QSqlDatabase db;
		static Sql* _instance;
		Sql(path* p=new path("fsclean.db"));
public:
		static Sql* getInstance();
		char sqlInsert(const Fichier &);
		void sqlDelDeletedFiles();
		void sqlRaz();
		bool sqlUpdate(Fichier);
        void Affiche();
        bool sqlCreateMD5();
        bool sqlDelete(Fichier &f);
        bool sqlSetMd5(Fichier &f);
		QSqlQueryModel *sqlSelect(string requete="SELECT * FROM Fichiers WHERE MD5 IN (SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1) ORDER BY MD5");
};

#endif // SQL_H
