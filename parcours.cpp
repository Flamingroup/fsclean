#include "parcours.h"
#include "fichier.h"
#include <fstream>

Parcours::Parcours(string chemin)
{
	path *p = stringToPath(chemin);
	if (p == NULL) return;
	ifstream config(p->string().c_str(), ios::in);
	if (config) {
		string ligne;
		char mode=0; // 0 : on attends de trouver liste blanche 1: on insère dans liste blanche 2: on insère dans liste noire
		while (getline(config, ligne)) {
			if (mode == 0 && ligne != "listeblanche")
				continue;
			if (mode == 0 && ligne == "listeblanche"){
				mode=1;
				continue;
			}
			if (mode == 1 && ligne != "listenoire") {
				addToWL(ligne);
			}
			if (mode == 1 && ligne == "listenoire") {
				mode=2;
				continue;
			}
			if (mode == 2 ) {
				addToBL(ligne);
			}
		}
		config.close();
	}
}

path* Parcours::stringToPath(string toTransform)
{
	string chemin;
	if (toTransform[0] == '~' || !exists(toTransform))
		return 0;
	path *p=new path(chemin);
	*p=canonical(*p);
	return p;
}

void Parcours::addToWL(string chemin)
{
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	if (listeblanche.find(tmp->string()) == listeblanche.end()){
		listeblanche[tmp->string()]=tmp;
	}
}

void Parcours::addToBL(string chemin)
{
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	if (listenoire.find(tmp->string()) == listenoire.end()){
		listenoire[tmp->string()]=tmp;
	}
}

void Parcours::rmvFromWL(string chemin)
{
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listeblanche.find(tmp->string());
	if (it != listeblanche.end()){
		delete (it->second);
		listeblanche.erase(it);
	}
}

void Parcours::rmvFromBL(string chemin)
{
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listenoire.find(tmp->string());
	if (it != listenoire.end()){
		delete (it->second);
		listenoire.erase(it);
	}
}

void Parcours::runAll()
{
	/* On execute le parcours résursif
	 * Si un résultat est un fichier :
	 *		on remplis les attributs d'un fichier
	 *		on insère le fichier7
	 * Sinon, c'est un dosier
	 *		on l'ajouet à la pile si il est pas dans la liste noire
	 * On recommence jusqu'a pile vide
	 *
	 */
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
		runFromPath(*it);
	}
}

void Parcours::runFromPath(const pair<string, path*>& thePair)
{
	list<path*> directories;
	directories.push_back(thePair.second);
	try {
		while (!directories.empty()) {
			if (exists(*directories.front())) {
				if (is_regular_file(*directories.front())) {
					Fichier *f = new Fichier(*directories.front());
					f->remplir();
					if (f->getid() == -1)
						f->insert();
					else f->updateIntoDB();
				}
				else if (is_directory(*directories.front()) && !isInBlacklist(*directories.front())){
					directories.push_back(directories.front());
				}
				else cout << directories.front()->string() << " exists, but is neither a regular file nor a directory\n";
			}
			else cout << directories.front()->string() << " does not exist anymore (deleted file during process ?";
			directories.pop_front();
		}
	}
	catch (const filesystem_error& ex) {
		cout << ex.what() << '\n';
	}
}

bool Parcours::isInBlacklist(path & p)
{
	if (listenoire.find(p.string()) == listenoire.end())
		return 0;
	return -1;
}
