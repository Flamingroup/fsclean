#ifndef FICHIER_H
#define FICHIER_H

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;

class Fichier
{
	private:
		int id;
		path chemin;
		string MD5; //ne pas remplir pour tous les fichiers
		string filenameTrime;
		u_int64_t poids;
        u_int32_t dateModif;
        //bool verifToujoursExistant
	public:
        Fichier();
        const u_int32_t& getDateModif() const;
        const string& getMD5() const;
        const path &getPath() const;
        void setPoids(u_int64_t pds);
        const u_int64_t& getPoids()const;
		void setid(int i);
        int getid()const;
		void setfilenameTrime(string s);
        const string& getfilenameTrime()const;
        void remplir(path p);
		string trim(string s);
};
#endif // FICHIER_H

/* TODO : tester dans un log :
 * enumeration correcte des éléments parcourus avec le find
 * remplir un fichier et l'afficher
 * recherche des doublons et stockage dans un log (chiant de parcourir un fichier texte)
*/
