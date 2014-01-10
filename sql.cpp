#include "sql.h"
#include <QVariant>
#include <QString>
#include <QSqlError>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <unistd.h>


Sql* Sql::_instance=NULL;

Sql::Sql(path* p) {   
	cout << "sqlCreate" << endl;
	if (exists(p->string())){
		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName((QString)p->string().c_str());
		if (!db.open()) {
			cerr << "Error occurred opening the database." << endl;
			throw 1;
		}
		db.close();
		return;
	}
	// Creation de la bdd
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName((QString)p->string().c_str());
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		unlink(p->string().c_str());
		throw 2;
	}
	// Insert table.
	QSqlQuery query(db);
	query.prepare("CREATE TABLE IF NOT EXISTS Fichiers (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, chemin TEXT UNIQUE, filenametrime TEXT, poids INTEGER, dateModif INTEGER, MD5 TEXT, stillexist INTEGER)");
	mutex.lock();
    if (!query.exec()) {
		cerr << "Error occurred creating table." << endl;
		db.close();
		unlink(p->string().c_str());
		mutex.unlock();
		throw 3;
	}
    mutex.unlock();
	db.close();
	return;
}

Sql* Sql::getInstance() {
	if(_instance == 0){
		try {
			_instance = new Sql ;
		}
		catch (int i) {
			cerr << "Erreur construction SQL, erreur :" << i << endl;
		}
	}
	return _instance;
}

char Sql::sqlInsert(const Fichier& f){
	cout << "sqlInsert" << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return -1;
	}
	QSqlQuery query(db);
	query.prepare("SELECT * FROM Fichiers WHERE chemin = :chemin");
	query.bindValue(":chemin", (QString)f.getChemin().string().c_str());
    mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred SELECT." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return -1;
	}
	if (query.next()) {
		if (f.getDateModif() != (quint32)query.value(4).toULongLong() || f.getPoids() != (quint64)query.value(3).toULongLong()) {
            //cout << "Supression des anciens fichiers" << endl;
			QSqlQuery query2(db);
			query2.prepare("DELETE FROM Fichiers WHERE id = :id");
			query2.bindValue(":id", query.value(0).toInt());
			if (!query2.exec()) {
				cerr << "Error occurred deleting." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
				mutex.unlock();
				return -1;
			}
		}
		else {
			QSqlQuery query2(db);
			query2.prepare("UPDATE Fichiers SET stillexist = 1 WHERE id = :id");
			query2.bindValue(":id", query.value(0).toInt());
			if (!query2.exec()) {
				cerr << "Error occurred while RAZing the flag." << query2.lastError().driverText().toStdString() << " " << query2.lastQuery().toStdString() << endl;
				mutex.unlock();
				return -1;
			}
            //cout << "Rien à faire, fichier déjà dans la base" << endl;
			db.close();
			cout << "noerror" << endl;
			mutex.unlock();
			return 0;
		}
	}
	mutex.unlock();
	query.prepare("INSERT INTO Fichiers (chemin, filenametrime, poids, dateModif, stillexist) VALUES (:chemin, :filenametrime, :poids, :date, 1)");
	query.bindValue(":chemin", (QString)f.getChemin().string().c_str());
	query.bindValue(":filenametrime", (QString)f.getfilenameTrime().c_str());
	query.bindValue(":poids", (QVariant)(quint64)f.getPoids());
	query.bindValue(":date", f.getDateModif());
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred inserting." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return -1;
	}
	mutex.unlock();
	db.close();
    //cout << "Fichier inséré ou MAJ sans erreurs" << endl;
	cout << "noerror" << endl;
	return 0;
}


void Sql::sqlDelDeletedFiles() {

	cout << "sqlDelDeletedFiles" << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return;
	}
	QSqlQuery query(db);
	query.prepare("DELETE FROM Fichiers WHERE stillexist = 0");
    mutex.lock();
        if (!query.exec()) {
		cerr << "Error occurred deleting deleted files." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return;
	}
    mutex.unlock();
	db.close();
    //cout << "noerror" << endl;
}

void Sql::sqlRaz() {

	cout << "sqlRAZ" << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return;
	}
	QSqlQuery query(db);
	query.prepare("UPDATE Fichiers SET stillexist = 0");
    mutex.lock();
    if (!query.exec()) {
		cerr << "Error occurred while RAZing the flag." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return;
	}
    mutex.unlock();
	db.close();
	cout << "noerror" << endl;
}

void Sql::Affiche(){

	cout << "=== SQL AFFICHE ===" << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return;
	}
	QSqlQuery qry(db);
    mutex.lock();
    if (!qry.exec("SELECT * FROM Fichiers")) {
		cerr << "Error occurred Affiching the database." << qry.lastError().driverText().toStdString() << " " << qry.lastQuery().toStdString() << endl;
		mutex.unlock();
		return;
	}
	while (qry.next()) {
		QString a = qry.value(0).toString();
		QString z = qry.value(1).toString();
		QString e = qry.value(2).toString();
		QString r = qry.value(3).toString();
		QString t = qry.value(4).toString();
		QString y = qry.value(5).toString();
		QString u = qry.value(6).toString();
		cout << a.toStdString() << endl
			 << z.toStdString() << endl
			 << e.toStdString() << endl
			 << r.toStdString() << endl
			 << t.toStdString() << endl
			 << y.toStdString() << endl
			 << u.toStdString() << endl << endl;
	}
    mutex.unlock();
	db.close();
}


bool Sql::sqlSetMd5(Fichier& f) {

    cout << "sqlSetMd5" << endl;
    if (f.getid() == -1 && f.getMD5() != "") {
        return false;
    }
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("UPDATE Fichiers SET MD5 = :MD5 WHERE id = :id");
    query.bindValue(":MD5", (QString)f.getMD5().c_str());
    query.bindValue(":id", f.getid());
    mutex.lock();
    if (!query.exec()) {
        cerr << "Error occurred while Updating the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;		
		mutex.unlock();
		return false;
    }
    mutex.unlock();
    db.close();
    //cout << "noerror" << endl;
    return true;
}

bool Sql::sqlDelete(Fichier& f) {

    cout<<"Sql::sqlDelete"<<endl;
    if (f.getid() == -1) {
        return false;
    }
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("DETETE FROM Fichiers WHERE id = :id");
    query.bindValue(":id", f.getid());
    mutex.lock();
    if (!query.exec()) {
        cerr << "Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
    }
    mutex.unlock();
    db.close();
    //cout << "noerror" << endl;
    return false;
}

bool Sql::sqlCreateMD5(){

    cout << "sql::sqlCreateMD5" << endl;
	list<Fichier*> lf;
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return false;
    }
	cout << "1" << endl;
    QSqlQuery query(db);
	query.prepare("SELECT * FROM Fichiers WHERE poids IN (SELECT poids FROM Fichiers WHERE 1 GROUP BY poids HAVING COUNT(poids)>1 AND MD5 IS NULL)");
	mutex.lock();
	cout << "1" << endl;
	if (!query.exec()) {
        cerr << "Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	cout << "1" << endl;
    while (query.next()) {
		cout << "Un tour " << endl;
		lf.push_back(new Fichier(query));
    }
	cout << "1" << endl;
    mutex.unlock();
	list<Fichier*>::iterator it=lf.begin();
	list<Fichier*>::iterator fin=lf.end();
	while (it != fin) {
		sqlSetMd5(**it);
		it=lf.erase(it);
	}
    db.close();
    //cout << "noerror" << endl;
    return false;
}


QSqlQueryModel* Sql::sqlSelect(string requete){

    cout<<"sql::sqlSelect"<<endl;
    QSqlQueryModel* model = new QSqlQueryModel();
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return 0;
    }
    QSqlQuery query(db);
    query.prepare(QString::fromStdString(requete));
    mutex.lock();
    if (!query.exec()) {
        cerr << "Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
        return 0;
    }
    model->setQuery(query);
    mutex.unlock();
    db.close();
    //cout << "noerror" << endl;
    return model;
}
