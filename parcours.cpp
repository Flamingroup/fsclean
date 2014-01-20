#include "parcours.h"
#include "fichier.h"
#include "sql.h"
#include <iostream>
#include <string>
#include <fstream>

int Parcours::AVANCE = 0;
Parcours* Parcours::_instance=NULL;
enum State{waitWL=0, scanWL=1,scanBL=2, getNbDBfic=3,errorStatus=4};
Parcours::Parcours(string chemin):cheminFicCfg(chemin) {
    cout<<"Parcours::Parcours()"<<endl;
    ifstream config(chemin.c_str(), ios_base::in);
    if (config) {
		string ligne;
        char etat=0;
        // 0 : on attends de trouver liste blanche
        // 1 : on insère dans liste blanche
        // 2 : on insère dans liste noire
        // 3 : on recupere le nombre de fichiers
        // 4 : on verifie si erreur lors de la derniere fermeture
        //cout<<"debut while : "<<endl;
		while (getline(config, ligne)) {
			if (ligne[0] == '#')
				continue;
			if (ligne == "scannercaches=1"){
				scannercaches=true;
				continue;
            }
            if (etat == waitWL && ligne != "listeblanche")
				continue;
            if (etat == waitWL && ligne == "listeblanche"){
                etat=scanWL;
				continue;
			}
            if (etat == scanWL && ligne != "listenoire") {
				cout<<"listeBlanche mise à jour"<<endl;
				addToWL(ligne);
				continue;
			}
            if (etat == scanWL && ligne == "listenoire") {
                etat=scanBL;
				continue;
			}
            if (etat == scanBL && ligne != "nombreapprox") {
                cout<<"listeNoire mise à jour"<<endl;
                addToBL(ligne);
				continue;
			}
            if (etat == scanBL && ligne == "nombreapprox") {
                etat=getNbDBfic;
				continue;
			}
            if (etat == getNbDBfic ) {
                if (ligne == "0"){
                    nbApprox=0;
					countApprox();
					continue;
				}
                else if(ligne == "error"){
                    etat = errorStatus;
                    continue;
                    }
				else {
					QString s=QString::fromStdString(ligne);
                    nbApprox=s.toDouble();
					AVANCE=100;
					continue;
                    }
			}
            if(etat == errorStatus){
                QString s = QString::fromStdString(ligne);
                if(s.toDouble()){
                    cout << "dernier scan interrompu" << endl;
                    countApprox();
                }
            }

		}
        //cout<<"fin while"<<endl;
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << chemin << ")." << endl;
	denom=nbApprox;
    AVANCE=(denom/nbApprox)*100;
	regenerateFicCfg();
	cout<<"fin Parcours::Parcours() " << nbApprox << endl;
}

Parcours* Parcours::getInstance(string chemin) {
	if(_instance == 0){
		try {
			_instance = new Parcours(chemin);
		}
		catch (int i) {
			cerr << "Erreur construction Parcours, erreur :" << i << endl;
		}
	}
	return _instance;
}
void Parcours::countApprox() {
    cout << "Parcours::CountApprox" << endl;
    nbApprox=0;
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
		runFromPath(*it, true);
	}
    AVANCE=100;
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
    mabase->sqlSetDossierDoublons();
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
    if (!countOnly){
        mabase->sqlInsertDossier(thePair.second->string());
    }
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
							if (f.getPoids() != 0)
								mabase->sqlInsert(f);
						}
                        ++nbApprox;
						AVANCE =(nbApprox/denom)*100;
					}
					else if(is_directory(*it) && !isInBlacklist(it->path()) && !isHidden(it->path())) {
						directories.push_back(new path(it->path()));
                        if (!countOnly){
                            mabase->sqlInsertDossier(it->path().string());
                        }
                        ++nbApprox;
                        AVANCE = (nbApprox / denom) * 100;
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

//error implanté pour eventuellement faire un reset auto de la config si plante
//mais non utilisé pour l'instant
void Parcours::resetFicCfg(int8_t error) {
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
               << "0" << endl
               << "error" << endl
               << error <<endl;
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << "./config.cfg" << ")." << endl;
	cout<<"fin Parcours::Parcours()"<<endl;
}

void Parcours::regenerateFicCfg(int err) {
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
        config << "error" << endl << err << endl;
		if(scannercaches){
			cout << "scannercaches=1" << endl;
		}
		config.close();
	}
	else cout << "Erreur ouverture fichier config (" << cheminFicCfg << ")." << endl;
	cout<<"fin Parcours::regenerateFicCfg()"<<endl;
}
