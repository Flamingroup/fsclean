#include "fichier.h"
#include <cctype>
#include <algorithm>

Fichier::Fichier(path p): chemin(p)
{
	id=-1;
}

void Fichier::setid(int i)
{
	id=i;
}

int Fichier::getid()
{
	return id;
}

void Fichier::setfilenameTrime(string s)
{
	filenameTrime=s;
}

string & Fichier::getfilenameTrime()
{
	return filenameTrime;
}

void Fichier::remplir()
{
	setfilenameTrime(trim((chemin.string())));
}

string Fichier::trim(string s)
{
	size_t position;
	transform(s.begin(), s.end(), s.begin(), (int(*)(int))toupper);
	while((position=s.find_first_of("'-._ ")) != s.npos)
		s.erase(position, 1);
	return s;
}
