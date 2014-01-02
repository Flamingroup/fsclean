#include "fichier.h"
#include <cctype>
#include <algorithm>
#include <boost/filesystem.hpp>

using namespace boost::filesystem;

Fichier::Fichier()//path p): chemin(p)
{
    id=-1;
}

const u_int32_t &Fichier::getDateModif() const
{
    return dateModif;
}

const string &Fichier::getMD5() const
{
    return MD5;
}

const path& Fichier::getChemin() const
{
    return chemin;
}

void Fichier::setPoids(u_int64_t pds)
{
    poids = pds;
}

const u_int64_t &Fichier::getPoids() const
{
    return poids;
}

void Fichier::setid(int i)
{
	id=i;
}

int Fichier::getid() const
{
	return id;
}

void Fichier::setfilenameTrime(string s)
{
	filenameTrime=s;
}

const string & Fichier::getfilenameTrime() const
{
	return filenameTrime;
}

void Fichier::remplir(path p)
{   //on nettoie le vieux fichier pour le réutiliser
	chemin.clear();
    MD5.clear();
	filenameTrime.clear();
    poids = 0;
    dateModif = 0;
    //on remplit
	chemin = canonical(p); // si segfault : faire par copie
	filenameTrime = trim(chemin.filename().string());
    poids = file_size(p);
    dateModif = last_write_time(p);
    //attention si le fichier n'est pas un fichier certains champs ne seront pas corrects
}

string Fichier::trim(string s)
{
	size_t position;
	transform(s.begin(), s.end(), s.begin(), (int(*)(int))tolower);
	while((position=s.find_first_of("'-._ ")) != s.npos)
		s.erase(position, 1);
	return s;
}

void Fichier::voir()
{
	cout << "id : " << id << endl;
	cout << "chemin : " << chemin.string() << endl;
	cout << "filenametrime : " << filenameTrime << endl;
	cout << "Poids : " << poids << endl;
	cout << "date : " << dateModif << endl;
	cout << "MD5 : " << MD5 << endl;
}
