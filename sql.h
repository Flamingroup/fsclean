#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"


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
};

#endif // SQL_H
