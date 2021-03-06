#include "sql.h"
#include "parcours.h"
#include <QVariant>
#include <QString>
#include <QMap>
#include <QSqlError>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <cstdio>

Sql* Sql::_instance=NULL;

Sql::Sql(path* p) {
	if (exists(p->string())){
		db = QSqlDatabase::addDatabase("QSQLITE");
		db.setDatabaseName((QString)p->string().c_str());
		if (!db.open()) {
            cerr << "    Error occurred opening the database." << endl;
			throw 1;
		}
		db.close();
		return;
	}
	// Creation de la bdd
	db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName((QString)p->string().c_str());
	if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
		unlink(p->string().c_str());
		throw 2;
	}
	// Insert table.
	QSqlQuery query(db);
	query.prepare("CREATE TABLE IF NOT EXISTS Fichiers (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, chemin TEXT UNIQUE, filenametrime TEXT, poids INTEGER, dateModif INTEGER, MD5 TEXT, stillexist INTEGER)");
	mutex.lock();
	if (!query.exec()) {
        cerr << "    Error occurred creating table." << endl;
		db.close();
		unlink(p->string().c_str());
		delete p;
		mutex.unlock();
		throw 3;
	}
	query.prepare("CREATE TABLE IF NOT EXISTS Dossiers (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL, chemin TEXT UNIQUE, stillexist INTEGER, isdoublon INTEGER, nbfic INTEGER, isparent INTEGER)");
	if (!query.exec()) {
        cerr << "    Error occurred creating table." << endl;
		db.close();
		unlink(p->string().c_str());
		delete p;
		mutex.unlock();
		throw 4;
	}
	mutex.unlock();
	db.close();
	delete p;
	return;
}

Sql* Sql::getInstance() {
	if(_instance == 0){
		try {
			_instance = new Sql ;
		}
		catch (int i) {
            cerr << "    Erreur construction SQL, erreur :" << i << endl;
		}
	}
	return _instance;
}

bool Sql::sqlInsert(const Fichier& f){
    //cout << "sqlInsert" << endl;
	if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
		return false;
	}
	QSqlQuery query(db);
	query.prepare("SELECT * FROM Fichiers WHERE chemin = :chemin");
	query.bindValue(":chemin", (QString)f.getChemin().string().c_str());
	mutex.lock();
	if (!query.exec()) {
        cerr << "    Error occurred SELECT." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	if (query.next()) {
		if (f.getDateModif() != (quint32)query.value(4).toULongLong() || f.getPoids() != (quint64)query.value(3).toULongLong()) {
			//cout << "Supression des anciens fichiers" << endl;
			QSqlQuery query2(db);
			query2.prepare("DELETE FROM Fichiers WHERE id = :id");
			query2.bindValue(":id", query.value(0).toInt());
			if (!query2.exec()) {
                cerr << "    Error occurred deleting." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
				mutex.unlock();
				return false;
			}
		}
		else {
			QSqlQuery query2(db);
			query2.prepare("UPDATE Fichiers SET stillexist = 1 WHERE id = :id");
			query2.bindValue(":id", query.value(0).toInt());
			if (!query2.exec()) {
                cerr << "    Error occurred while Updating the flag." << query2.lastError().driverText().toStdString() << " " << query2.lastQuery().toStdString() << endl;
				mutex.unlock();
				return false;
			}
			//cout << "Rien à faire, fichier déjà dans la base" << endl;
			db.close();
            //cout << "noerror" << endl;
			mutex.unlock();
			return true;
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
        cerr << "    Error occurred inserting." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	mutex.unlock();
	db.close();
	//cout << "Fichier inséré ou MAJ sans erreurs" << endl;
    //cout << "noerror" << endl;
	return true;
}

bool Sql::sqlInsertDossier(const string& str){
    //cout << "sqlInsertDossier de "<< str << endl;
	if (!db.open()) {
		cerr << "Error occurred opening the database." << endl;
		return false;
	}
	QSqlQuery query(db);
	query.prepare("SELECT * FROM Dossiers WHERE chemin = :chemin");
    query.bindValue(":chemin", QString(str.c_str()));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred SELECTing Dossiers." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	if (query.next()) {
		QSqlQuery query2(db);
		query2.prepare("UPDATE Dossiers SET stillexist = 1, isdoublon = 0, isparent = 0 WHERE id = :id");
		query2.bindValue(":id", query.value(0).toInt());
		if (!query2.exec()) {
			cerr << "Error occurred while Updating the flag of dossiers." << query2.lastError().driverText().toStdString() << " " << query2.lastQuery().toStdString() << endl;
			mutex.unlock();
			return false;
		}
		//cout << "Rien à faire, fichier déjà dans la base" << endl;
		db.close();
        //cout << "noerror" << endl;
		mutex.unlock();
		return true;
	}
	mutex.unlock();
	query.prepare("INSERT INTO Dossiers (chemin, stillexist, isdoublon, isparent) VALUES (:chemin, 1, 0, 0)");
	query.bindValue(":chemin", QString(str.c_str()));
	mutex.lock();
	if (!query.exec()) {
		cerr << "Error occurred inserting in dossiers." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	mutex.unlock();
	db.close();
	//cout << "Dossier inséré sans erreur" << endl;
    //cout << "noerror" << endl;
	return true;
}

bool Sql::sqlDelDeletedFiles() {
    //cout << "sqlDelDeletedFiles" << endl;
	if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
		return false;
	}
	QSqlQuery query(db);
	query.prepare("DELETE FROM Fichiers WHERE stillexist = 0");
	mutex.lock();
	if (!query.exec()) {
        cerr << "    Error occurred deleting deleted files." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	query.prepare("DELETE FROM Dossiers WHERE stillexist = 0");
	if (!query.exec()) {
        cerr << "    Error occurred deleting deleted files." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
    mutex.unlock();
	db.close();
	return true;
    //cout << "noerror" << endl;
}

bool Sql::sqlRaz() {
    //cout << "sqlRAZ" << endl;
	if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
		return false;
	}
	QSqlQuery query(db);
	query.prepare("UPDATE Fichiers SET stillexist = 0");
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while RAZing the flag." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
	query.prepare("UPDATE Dossiers SET stillexist = 0, isdoublon = 0, isparent = 0");
	if (!query.exec()) {
        cerr << "    Error occurred while RAZing the flag." << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
    mutex.unlock();
	db.close();
	return true;
    //cout << "noerror" << endl;
}

bool Sql::Affiche(string commande){
	if (!db.open()) {
		cerr << "    Error occurred opening the database." << endl;
		return false;
	}
	if (commande == "MD5") {
		cout << "=== Doublons certains (MD5) ===" << endl;
		QSqlQuery qry(db);
		mutex.lock();
		if (!qry.exec(MD5.c_str())) {
			cerr << "    Error occurred Affiching the database." << qry.lastError().driverText().toStdString() << " " << qry.lastQuery().toStdString() << endl;
			mutex.unlock();
			return false;
		}
		printf("%80s %20s %11s   %10s\n",
			   "Chemin",
			   "Filenametrime",
			   "Poids",
			   "Date modif"
			   );
		while (qry.next()) {
			printf("%80s %20s %11s   %10s\n",
				   qry.value(0).toString().toStdString().c_str(),
				   qry.value(1).toString().toStdString().c_str(),
				   qry.value(2).toString().toStdString().c_str(),
				   qry.value(3).toString().toStdString().c_str()
				   );
		}
		cout << endl;
		mutex.unlock();
	}
	else if (commande == "nom"){
		cout << "=== Doublons nom similaire ===" << endl;
		QSqlQuery qry(db);
		mutex.lock();
		if (!qry.exec(filenametrime.c_str())) {
			cerr << "    Error occurred Affiching the database." << qry.lastError().driverText().toStdString() << " " << qry.lastQuery().toStdString() << endl;
			mutex.unlock();
			return false;
		}
		printf("%80s %20s %11s   %10s\n",
			   "Chemin",
			   "Filenametrime",
			   "Poids",
			   "Date modif"
			   );
		while (qry.next()) {
			printf("%80s %20s %11s   %10s\n",
				   qry.value(0).toString().toStdString().c_str(),
				   qry.value(1).toString().toStdString().c_str(),
				   qry.value(2).toString().toStdString().c_str(),
				   qry.value(3).toString().toStdString().c_str()
				   );
		}
		cout << endl;
		mutex.unlock();
	}
	else if (commande == "dossier"){
		cout << "=== Doublons dossiers potentiels ===" << endl;
		QSqlQuery qry(db);
		mutex.lock();
		if (!qry.exec(dossier.c_str())) {
			cerr << "    Error occurred Affiching the database." << qry.lastError().driverText().toStdString() << " " << qry.lastQuery().toStdString() << endl;
			mutex.unlock();
			return false;
		}
		printf("%80s %20s\n",
			   "Chemin",
			   "Nombres de fichiers"
			   );
		while (qry.next()) {
			printf("%80s %20s\n",
				   qry.value(0).toString().toStdString().c_str(),
				   qry.value(1).toString().toStdString().c_str()
				   );
		}
		cout << endl;
		mutex.unlock();
	}
	db.close();
	return true;
}

bool Sql::sqlSetMd5(Fichier& f) {

    //cout << "sqlSetMd5" << endl;
    if (f.getid() == -1 && f.getMD5() != "") {
        return false;
    }
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return false;
    }
    QSqlQuery query(db);
	query.prepare("UPDATE Fichiers SET MD5 = :MD5 WHERE id = :id");
    query.bindValue(":MD5", (QString)f.getMD5().c_str());
    query.bindValue(":id", f.getid());
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while Updating the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
    }
    mutex.unlock();
    db.close();
    //cout << "noerror" << endl;
    return true;
}

bool Sql::sqlDelete(string chemin) {
    //cout<<"Sql::sqlDelete"<<endl;
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return false;
    }
    QSqlQuery query(db);
    query.prepare("DELETE FROM Fichiers WHERE chemin = :chemin");
	query.bindValue(":chemin", QString(chemin.c_str()));
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
    }
    mutex.unlock();
    db.close();
    //cout << "noerror" << endl;
    return false;
}

bool Sql::sqlCreateMD5(){
    //cout << "sql::sqlCreateMD5" << endl;
	list<Fichier*> lf;
	Fichier* tmp;
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return false;
	}
    QSqlQuery query(db);
	query.prepare("SELECT * FROM Fichiers WHERE poids IN (SELECT poids FROM Fichiers WHERE 1 GROUP BY poids HAVING COUNT(poids)>1 AND MD5 IS NULL)");
	mutex.lock();
	if (!query.exec()) {
        cerr << "    Error occurred while Deleting the file. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		return false;
	}
    while (query.next() && Parcours::STOP == false) {
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
		delete (*it);
		it=lf.erase(it);
	}
    db.close();
    //cout << "noerror" << endl;
    return false;
}

bool Sql::isDossierDoublon(const string& chemin){
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return false;
    }
    string concatene=chemin;
	concatene+="/%";
    QSqlQuery query(db);
    query.prepare(QString("SELECT COUNT(*) FROM Fichiers WHERE chemin LIKE :chemin and MD5 IS NULL"));
    query.bindValue(":chemin", QString(concatene.c_str()));
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while selecting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
        mutex.unlock();
        return false;
    }
    if (query.next()) {
        if (query.value(0).toInt() == 0){
            query.prepare(QString("SELECT COUNT(*) FROM Fichiers WHERE chemin LIKE :chemin"));
            query.bindValue(":chemin", QString(concatene.c_str()));
            if (!query.exec()) {
                cerr << "    Error occurred while selecting doublons MD5 bis. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
                mutex.unlock();
                return false;
            }
            if (query.next()){
                int j= query.value(0).toInt();
                query.prepare(QString("UPDATE Dossiers SET nbfic = :toset WHERE chemin = :chemin"));
                query.bindValue(":toset", j);
                query.bindValue(":chemin", QString(chemin.c_str()));
                if (!query.exec()) {
                    cerr << "    Error occurred while updating nbfic MD5 bis. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
                    mutex.unlock();
                    return false;
                }
            }
            mutex.unlock();
            db.close();
            return true;
        }
        else {
            mutex.unlock();
            db.close();
            return false;
        }
    }
    return false;
}

bool Sql::sqlSetDossierDoublons(){
    //cout << "sql::sqlSetDossierDoublons" << endl;
    list<string> ls;
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return 0;
    }
    QSqlQuery query(db);
    query.prepare(QString("SELECT chemin FROM Dossiers"));
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while setting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
        mutex.unlock();
        return 0;
    }
    while (query.next()) {
        ls.push_back(query.value(0).toString().toStdString());
    }
    db.close();
    mutex.unlock();
    list<string>::iterator it = ls.begin();
    list<string>::iterator fin = ls.end();
    while (it != fin) {
        if (isDossierDoublon(*it)){
            if (!db.open()) {
                cerr << "    Error occurred opening the database." << endl;
                return 0;
            }
            QSqlQuery query2(db);
			query2.prepare(QString("UPDATE Dossiers SET isdoublon = 1, isparent = 1 WHERE chemin = :chemin"));
            query2.bindValue(":chemin", QString((*it).c_str()));
            mutex.lock();
            if (!query2.exec()) {
                cerr << "    Error occurred while Updating flag doublons dossier. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
            }
            db.close();
            mutex.unlock();
            ++it;
        }
        else {
            it = ls.erase(it);
        }
    }
	it = ls.begin();
	while (it != fin) {
		if (!isDossierParent(*it)){
			if (!db.open()) {
				cerr << "    Error occurred opening the database." << endl;
				return 0;
			}
			QSqlQuery query2(db);
			query2.prepare(QString("UPDATE Dossiers SET isparent = 0 WHERE chemin = :chemin"));
			query2.bindValue(":chemin", QString((*it).c_str()));
			mutex.lock();
			if (!query2.exec()) {
				cerr << "    Error occurred while Updating flag doublons dossier. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
			}
			db.close();
			mutex.unlock();
			++it;
		}
		else {
			it = ls.erase(it);
		}
	}
    db.close();
    //cout << "noerror" << endl;
    return true;
}


QSqlQueryModel* Sql::sqlSelectDoublons(string select){
    //cout << "sql::sqlSelectDoublons" << endl << "query = " << select <<endl;
    QSqlQueryModel* model = new QSqlQueryModel();
    if (!db.open()) {
        cerr << "    Error occurred opening the database." << endl;
        return 0;
    }
    QSqlQuery query(db);
    query.prepare(QString(select.c_str()));
    mutex.lock();
    if (!query.exec()) {
        cerr << "    Error occurred while selecting doublons. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
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

bool Sql::isDossierParent(const string& chemin){
	if (!db.open()) {
		cerr << "    Error occurred opening the database." << endl;
		return false;
	}
	QSqlQuery query(db);
	path *p=Parcours::stringToPath(chemin);
	query.prepare(QString("SELECT * FROM Dossiers WHERE chemin = :chemin AND isdoublon = 1"));
	query.bindValue(":chemin", QString(p->branch_path().string().c_str()));
	mutex.lock();
	if (!query.exec()) {
		cerr << "    Error occurred while selecting doublons MD5. " << query.lastError().driverText().toStdString() << " " << query.lastQuery().toStdString() << endl;
		mutex.unlock();
		db.close();
		delete p;
		return false;
	}
	if (query.next()) {
		mutex.unlock();
		db.close();
		delete p;
		return false;
	}
	else {
		mutex.unlock();
		db.close();
		delete p;
		return true;
	}
}
