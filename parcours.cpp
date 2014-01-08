#include "parcours.h"
#include "fichier.h"
#include "sql.h"
#include <iostream>
#include <string>
#include <fstream>

int Parcours::AVANCE = 0;
Parcours::Parcours(string chemin):cheminFicCfg(chemin) {
    cout<<"Parcours::Parcours()"<<endl;
    ifstream config(chemin.c_str(), ios_base::in);
    if (config) {
		string ligne;
        char mode=0;
        // 0 : on attends de trouver liste blanche
        // 1: on insère dans liste blanche
        // 2: on insère dans liste noire
        //cout<<"debut while : "<<endl;
		while (getline(config, ligne)) {
			if (ligne[0] == '#')
				continue;
			if (ligne == "scannercaches=1"){
				scannercaches=true;
				continue;
			}
			if (mode == 0 && ligne != "listeblanche")
				continue;
			if (mode == 0 && ligne == "listeblanche"){
				mode=1;
				continue;
			}
			if (mode == 1 && ligne != "listenoire") {
				cout<<"listeBlanche mise à jour"<<endl;
				addToWL(ligne);
				continue;
			}
			if (mode == 1 && ligne == "listenoire") {
				mode=2;
				continue;
			}
			if (mode == 2 && ligne != "nombreapprox") {
                cout<<"listeNoire mise à jour"<<endl;
                addToBL(ligne);
				continue;
			}
			if (mode == 2 && ligne == "nombreapprox") {
				mode=3;
				continue;
			}
			if (mode == 3 ) {
				if (ligne == "0"){
                    nbApprox=0;
					countApprox();
					continue;
				}
				else {
					QString s=QString::fromStdString(ligne);
                    nbApprox=s.toDouble();
					AVANCE=100;
					continue;
				}
			}

		}
        //cout<<"fin while"<<endl;
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << chemin << ")." << endl;
	denom=nbApprox;
	regenerateFicCfg();
    cout<<"fin Parcours::Parcours() " << nbApprox << endl;
}

void Parcours::countApprox() {
    cout << "Parcours::CountApprox" << endl;
    nbApprox=0;
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
		runFromPath(*it, true);
	}
}

path* Parcours::stringToPath(string toTransform, bool verifExist) {
    if(toTransform.length() == 0) return NULL;
	if (toTransform[0] == '~') {
		if (toTransform.length() == 1) {
			toTransform=secure_getenv("HOME");
		}
		else {
			toTransform.erase(toTransform.begin());
			toTransform=secure_getenv("HOME")+toTransform;
		}
        cout << "dans ~ : *" << toTransform << "*";
	}
	else if (toTransform[0] != '/'){
		toTransform=boost::filesystem::initial_path().string()+'/'+toTransform;
		cout << "dans / : *" << toTransform << "*";
	}
	if (verifExist && !exists(toTransform))
		return 0;
	path *p=new path(toTransform);
	*p=canonical(*p);
    return p;
}

void Parcours::voirWL() {
    cout<<"contenu Wlist : "<<endl;
    map<string, path*>::iterator it = listeblanche.begin();
    map<string, path*>::iterator fin = listeblanche.end();
    for(;it!=fin;it++){
        cout<<"string="<<(*it).first<<endl<<"path="<<((*it).second)->string()<<endl;
    }
}

void Parcours::voirBL() {
    cout<<"contenu Blist : "<<endl;
    map<string, path*>::iterator it = listenoire.begin();
    map<string, path*>::iterator fin = listenoire.end();
    for(;it!=fin;it++){
        cout<<"string="<<(*it).first<<endl<<"path="<<((*it).second)->string()<<endl;
    }
}

void Parcours::addToWL(string chemin) {
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "addToWL : path tmp=" << tmp->string() <<endl;
	if (listeblanche.find(tmp->string()) == listeblanche.end()) {
		listeblanche[tmp->string()]=tmp;
        cout<<"ajout fait"<<endl;
    }
    else cout<<"deja dans WL"<<endl;
}

void Parcours::addToBL(string chemin) {
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "addToBL : path tmp=" << tmp->string() <<endl;
	if (listenoire.find(tmp->string()) == listenoire.end()) {
		listenoire[tmp->string()]=tmp;
        cout<<"ajout fait"<<endl;
    }
    else cout<<"deja dans BL"<<endl;
}

void Parcours::rmvFromWL(string chemin) {
    cout<<"rmvFromWL : "<<chemin<<endl;
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listeblanche.find(tmp->string());
	if (it != listeblanche.end()){
		delete (it->second);
		listeblanche.erase(it);
    }

}

void Parcours::rmvFromBL(string chemin) {
    cout<<"rmvFromBL : "<<chemin<<endl;
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listenoire.find(tmp->string());
	if (it != listenoire.end()){
		delete (it->second);
		listenoire.erase(it);
	}
}

void Parcours::runAll() {
	/* On execute le parcours résursif
	 * Si un résultat est un fichier :
	 *		on remplis les attributs d'un fichier
	 *		on insère le fichier7
     * Sinon, c'est un dossier
	 *		on l'ajouet à la pile si il est pas dans la liste noire
	 * On recommence jusqu'a pile vide
	 *
	 */
	nbApprox=0;
	AVANCE = (nbApprox/denom)*100;
    cout << "Parcours::runAll()" << endl;
	Sql* mabase=Sql::getInstance();
	mabase->sqlRaz();
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
        cout<<"Parcours::runall() : "<<(*it).first<<endl<<" va etre inspecté"<<endl;
		runFromPath(*it);
	}
	mabase->sqlDelDeletedFiles();
	mabase->sqlCreateMD5();
	regenerateFicCfg();
}

void Parcours::runFromPath(const pair<string, path*>& thePair, bool countOnly) {
    cout<<"Parcours::runFromPath()"<<endl;
    Sql* mabase;
    if (!countOnly){
        mabase=Sql::getInstance();
    }
	list<path*> directories;
	directories.push_back(new path(*thePair.second));
	Fichier f;
	while(!directories.empty()){
		try{
			if(exists(*directories.front())){
				directory_iterator it(*directories.front());
				directory_iterator end;
				for (; it != end; ++it){
					if(is_regular_file(*it) && !isInBlacklist(it->path()) && !isHidden(it->path())) {
						if (!countOnly){
							f.remplir(it->path());
							mabase->sqlInsert(f);
						}
                        ++nbApprox;
						AVANCE =(nbApprox/denom)*100;
					}
					else if(is_directory(*it) && !isInBlacklist(it->path()) && !isHidden(it->path())) {
						directories.push_back(new path(it->path()));
					}
				}
			}
			delete directories.front();
			directories.pop_front();
		}
		catch (const filesystem_error& ex)
		{
			delete directories.front();
			directories.pop_front();
		}
	}
}

bool Parcours::isInBlacklist(const path & p) {
	if (listenoire.find(p.string()) == listenoire.end())
		return false;
	return true;
}

bool Parcours::isHidden(const path& p) {
	if (p.string().find("/.") == p.string().npos)
		return false;
	return true;
}

void Parcours::resetFicCfg() {
	cout<<"Parcours::resetFicCfg()\n"<<endl;
	ofstream config(cheminFicCfg, ios_base::out);
	if (config) {
		config << "listeblanche" << endl
			   << "/home" << endl
			   << "/media" << endl
			   << "listenoire" << endl
			   << "/proc" << endl
			   << "/boot" << endl
			   << "/etc" << endl
			   << "nombreapprox" << endl
			   << "0" << endl;
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << "./config.cfg" << ")." << endl;
	cout<<"fin Parcours::Parcours()"<<endl;
}

void Parcours::regenerateFicCfg() {
	cout<<"Parcours::regenerateFicCfg()\n"<<endl;
	ofstream config(cheminFicCfg, ios_base::out);
	if (config) {
		map<string, path*>::iterator it = listeblanche.begin();
		map<string, path*>::iterator fin = listeblanche.end();
		config<<"listeblanche"<<endl;
		for(;it!=fin;it++){
			config << (*it).first << endl;
		}
		it = listenoire.begin();
		fin = listenoire.end();
		config<<"listenoire"<<endl;
		for(;it!=fin;it++){
			config << (*it).first << endl;
		}
        config << "nombreapprox" << endl << nbApprox << endl;
		if(scannercaches){
			cout << "scannercaches=1" << endl;
		}
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << cheminFicCfg << ")." << endl;
	cout<<"fin Parcours::regenerateFicCfg()"<<endl;
}
