#ifndef FICHIER_H
#define FICHIER_H
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;

class Fichier
{
	private:
		int id;
		path chemin;
		string filenameTrime;
		u_int64_t poids;
		u_int32_t dateModif;
		string MD5; //ne pas remplir pour tous les fichiers
        //bool verifToujoursExistant
	public:
		Fichier();
		Fichier(QSqlQuery&);
		void setid(int i);
		int getid() const;
		const path& getChemin() const;
		void setfilenameTrime(string s);
		const string& getfilenameTrime() const;
		void setPoids(u_int64_t pds);
		const u_int64_t& getPoids() const;
		const u_int32_t& getDateModif() const;
		const string& getMD5() const;

		void remplir(path p);
		string trim(string s);
        void voir();
        bool remplir(const QSqlQuery &query);
        void calcMD5();
};
#endif // FICHIER_H

/* TODO : tester dans un log :
 * enumeration correcte des éléments parcourus avec le find
 * remplir un fichier et l'afficher
 * recherche des doublons et stockage dans un log (chiant de parcourir un fichier texte)
*/
