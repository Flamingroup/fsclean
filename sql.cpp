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
	query.prepare("CREATE TABLE IF NOT EXISTS Dossiers (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, chemin TEXT UNIQUE, stillexist INTEGER, isdoublon INTEGER)");
	if (!query.exec()) {
		cerr << "Error occurred creating table." << endl;
		db.close();
		unlink(p->string().c_str());
		mutex.unlock();
		throw 4;
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
				cerr << "Error occurred while Updating the flag." << query2.lastError().driverText().toStdString() << " " << query2.lastQuery().toStdString() << endl;
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
	query.bindValue(":chemin", QString(f.getChemin().string().c_str()));
	query.bindValue(":filenametrime", QString(f.getfilenameTrime().c_str()));
	query.bindValue(":poids", QVariant(quint64(f.getPoids())));
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

char Sql::sqlInsertDossier(const string& str){
	cout << "sqlInsertDossier" << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return -1;
	}
	QSqlQuery query(db);
	query.prepare("SELECT * FROM Dossiers WHERE chemin = :chemin");
	query.bindValue(":chemin", QString(str.c_str());
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred SELECTing Dossiers." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return -1;
	}
	if (query.next()) {
		QSqlQuery query2(db);
		query2.prepare("UPDATE Dossiers SET stillexist = 1, isdoublon = 0 WHERE id = :id");
		query2.bindValue(":id", query.value(0).toInt());
		if (!query2.exec()) {
			cerr << "Error occurred while Updating the flag of dossiers." << query2.lastError().driverText().toStdString() << " " << query2.lastQuery().toStdString() << endl;
			mutex.unlock();
			return -1;
		}
		//cout << "Rien à faire, fichier déjà dans la base" << endl;
		db.close();
		cout << "noerror" << endl;
		mutex.unlock();
		return 0;
	}
	mutex.unlock();
	query.prepare("INSERT INTO Dossiers (chemin, stillexist, isdoublon) VALUES (:chemin, 1, 0)");
	query.bindValue(":chemin", QString(str.c_str()));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred inserting in dossiers." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return -1;
	}
	mutex.unlock();
	db.close();
	//cout << "Dossier inséré sans erreur" << endl;
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
	query.prepare("DELETE FROM Dossiers WHERE stillexist = 0");
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
	query.prepare("UPDATE Dossiers SET stillexist = 0, isdoublon = 0");
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

bool Sql::sqlDelete(string chemin) {
    cout<<"Sql::sqlDelete"<<endl;
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return false;
    }
    QSqlQuery query(db);
	query.prepare("DETETE FROM Fichiers WHERE chemin = :chemin");
	query.bindValue(":chemin", QString(chemin.c_str()));
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
	Fichier* tmp;
    if (!db.open()) {
        cerr << "Error occurred opening the database." << endl;
        return false;
	}
    QSqlQuery query(db);
	query.prepare("SELECT * FROM Fichiers WHERE poids IN (SELECT poids FROM Fichiers WHERE 1 GROUP BY poids HAVING COUNT(poids)>1 AND MD5 IS NULL)");
	mutex.lock();
	if (!query.exec()) {
        cerr << "Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	while (query.next()) {
		try {
			tmp=new Fichier(query);
		}
		catch (int i) {
			continue;
		}
		lf.push_back(tmp);
	}
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

bool Sql::isDossierDoublon(const string& chemin){
	QSqlQuery query(db);
	query.prepare(QString("SELECT COUNT(*), FROM Fichiers WHERE chemin LIKE :chemin% and MD5 IS NULL))"));
	query.bindValue(":chemin", QString(chemin.c_str()));
	if (!query.exec()) {
		cerr << "Error occurred while selecting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return 0;
	}
	db.close();
	if (query.value(0).toInt() == 0){
		return false;
	}
	else return true;
	db.close();
}

bool Sql::sqlSetDossierDoublons(){
	cout << "sql::sqlSetDossierDoublons" << endl;
	list<string> ls;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return 0;
	}
	QSqlQuery query(db);
	query.prepare(QString("SELECT chemin, FROM Dossiers"));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred while setting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return 0;
	}
	while (query.next()) {
		lf.push_back(query.value(0));
	}
	mutex.unlock();
	list<string>::iterator it = ls.begin();
	list<string>::iterator fin = ls.end();
	while (it != fin) {
		if (isDossierDoublon(*it)){
			query.prepare(QString("UPDATE Dossiers SET isdoublon = 1 WHERE chemin = :chemin"));
			query.bindValue(":chemin", QString((*it).c_str()));
			mutex.lock();
			if (!query.exec()) {
				cerr << "Error occurred while Updating flag doublons dossier. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
			}
			mutex.unlock();
		}
		it = ls.erase(it);
	}
	db.close();
	//cout << "noerror" << endl;
	return model;
}

QSqlQueryModel* Sql::sqlSelectDoublonsMD5(){
	cout << "sql::sqlSelectDoublonsMD5" << endl;
	QSqlQueryModel* model = new QSqlQueryModel();
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return 0;
	}
	QSqlQuery query(db);
	query.prepare(QString("SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers WHERE(MD5 IN(SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1))"));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred while selecting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return 0;
	}
	model->setQuery(query);
	while (model->canFetchMore()){
		model->fetchMore();
	}
	mutex.unlock();
	db.close();
	//cout << "noerror" << endl;
	return model;
}

QSqlQueryModel* Sql::sqlSelectDoublonsFilenametrime(){
	cout << "sql::sqlSelectDoublonsFilenametrime" << endl;
	QSqlQueryModel* model = new QSqlQueryModel();
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return 0;
	}
	QSqlQuery query(db);
	query.prepare(QString("SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers filenametrime IN (SELECT filenametrime FROM Fichiers WHERE 1 GROUP BY filenametrime HAVING COUNT(filenametrime)>1) ORDER BY MD5,chemin"));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred while selecting doublons Filenametrime. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return 0;
	}
	model->setQuery(query);
	while (model->canFetchMore()){
		model->fetchMore();
	}
	mutex.unlock();
	db.close();
	//cout << "noerror" << endl;
	return model;
}

QSqlQueryModel* Sql::sqlSelectDossiersDoublons(){
	cout << "sql::sqlSelectDossiersDoublons" << endl;
	QSqlQueryModel* model = new QSqlQueryModel();
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return 0;
	}
	QSqlQuery query(db);
	query.prepare(QString("SELECT chemin FROM Dossiers WHERE isdoublon = 1"));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred while selecting doublons Dossier. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return 0;
	}
	model->setQuery(query);
	while (model->canFetchMore()){
		model->fetchMore();
	}
	mutex.unlock();
	db.close();
	//cout << "noerror" << endl;
	return model;
}

