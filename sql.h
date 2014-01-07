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
        QSqlQueryModel *sqlSelect(string requete);
};

#endif // SQL_H
