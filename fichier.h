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
		string MD5;
		string filenameTrime;
		u_int64_t poids;
	public:
		Fichier(path p);
		void setid(int i);
		int getid();
		void setfilenameTrime(string s);
		string& getfilenameTrime();
		void remplir();
		void insert();
		void updateIntoDB();
		string trim(string s);
};

#endif // FICHIER_H
