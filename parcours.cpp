#include "parcours.h"
#include "fichier.h"
#include "sql.h"
#include <iostream>
#include <string>
#include <fstream>

char Parcours::AVANCE = 0;
bool Parcours::STOP = false;
Parcours* Parcours::_instance=NULL;

enum State{waitWL=0, scanWL=1,scanBL=2, getNbDBfic=3,errorStatus=4};
// 0 : on attends de trouver liste blanche
// 1 : on insère dans liste blanche
// 2 : on insère dans liste noire
// 3 : on recupere le nombre de fichiers
// 4 : on verifie si erreur lors de la derniere fermeture

Parcours::Parcours(string chemin):cheminFicCfg(chemin) {
    cout<<"Création Parcours..."<<endl;
    ifstream config(chemin.c_str(), ios_base::in);
    if (config) {
		string ligne;
        char etat=waitWL;
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
                cout<<"    ListeBlanche mise à jour"<<endl;
				addToWL(ligne);
				continue;
			}
            if (etat == scanWL && ligne == "listenoire") {
                etat=scanBL;
				continue;
			}
            if (etat == scanBL && ligne != "nombreapprox") {
                cout<<"    ListeNoire mise à jour"<<endl;
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
                    cout << "    Dernier scan interrompu" << endl;
                    countApprox();
                }
            }

        }
		config.close();
	}
    else {
        cerr << "    Erreur ouverture fichier config (" << chemin << ")." << endl;
        resetFicCfg();
        Parcours::getInstance(chemin);
        return;
    }
    denom=nbApprox;
    AVANCE=(denom/nbApprox)*100;
	regenerateFicCfg();
    cout<<"    Parcours construit." << endl;
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
    cout << "Calcul nombre de fichiers à scanner..." << endl;
    nbApprox=0;
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
		runFromPath(*it, true);
	}
    AVANCE=100;
    cout << "    " << nbApprox << " fichiers trouvés." <<endl;
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
        //cout << "dans ~ : *" << toTransform << "*";
	}
	else if (toTransform[0] != '/'){
		toTransform=boost::filesystem::initial_path().string()+'/'+toTransform;
        //cout << "dans / : *" << toTransform << "*";
	}
	if (verifExist && !exists(toTransform))
		return 0;
	path *p=new path(toTransform);
	*p=canonical(*p);
    return p;
}

void Parcours::voirWL() {
    cout<<"Contenu liste blanche : "<<endl;
    map<string, path*>::iterator it = listeblanche.begin();
    map<string, path*>::iterator fin = listeblanche.end();
    for(;it!=fin;it++){
        cout<< "    " <<(*it).first<<endl;
    }
}

void Parcours::voirBL() {
    cout<<"Contenu liste noire : "<<endl;
    map<string, path*>::iterator it = listenoire.begin();
    map<string, path*>::iterator fin = listenoire.end();
    for(;it!=fin;it++){
        cout<<"    "<<(*it).first<<endl;
    }
}

void Parcours::addToWL(string chemin) {
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "    Ajout liste blanche : " << tmp->string() <<endl;
    if (listeblanche.find(tmp->string()) == listeblanche.end())
		listeblanche[tmp->string()]=tmp;
    else cout << "    " << tmp->string() << " déjà en liste blanche."<<endl;
}

void Parcours::addToBL(string chemin) {
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "    Ajout liste noire : " << tmp->string() <<endl;
    if (listenoire.find(tmp->string()) == listenoire.end())
        listenoire[tmp->string()]=tmp;
    else cout << "    " << tmp->string() << " déjà en liste noire." <<endl;
}

void Parcours::rmvFromWL(string chemin) {
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listeblanche.find(tmp->string());
	if (it != listeblanche.end()){
        cout << tmp->string() << " oté de la liste blanche." << endl;
		delete (it->second);
		listeblanche.erase(it);
    }

}

void Parcours::rmvFromBL(string chemin) {
	path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
	map<string, path*>::iterator it=listenoire.find(tmp->string());
	if (it != listenoire.end()){
        cout << tmp->string() << " oté de la liste noire." << endl;
		delete (it->second);
		listenoire.erase(it);
	}
}

void Parcours::runAll() {
	nbApprox=0;
	AVANCE = (nbApprox/denom)*100;
	Sql* mabase=Sql::getInstance();
    mabase->sqlRaz();
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
        if(!STOP)
            runFromPath(*it);
    }
    cout << "    Suppression des fichiers non existants..." << endl;
    mabase->sqlDelDeletedFiles();
    if(!STOP){
        cout << "    Calcul des clés MD5..." << endl;
        mabase->sqlCreateMD5();
    }
    cout << "    Recherche des dossiers doublons..." << endl;
    mabase->sqlSetDossierDoublons();
    //cout << "    Mise à jour de config.cfg" << endl;
    //regenerateFicCfg();
}

void Parcours::runFromPath(const pair<string, path*>& thePair, bool countOnly) {
    //cout<<"Parcours::runFromPath()"<<endl;
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
    while(!directories.empty() && STOP == false){
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
                        AVANCE =(nbApprox / denom)*100;
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
               << "/tmp" << endl
               << "/dev" << endl
               << "/sys" << endl
			   << "nombreapprox" << endl
               << "0" << endl
               << "error" << endl
               << error <<endl;
        config.close();
        listeblanche.clear();
        listenoire.clear();
        addToWL("/home");
        addToWL("/media");
        addToBL("/proc");
        addToBL("/boot");
        addToBL("/etc");
        addToBL("/tmp");
        addToBL("/dev");
        addToBL("/sys");
	}
	else cout << "Erreur ouverture fichier config (" << "./config.cfg" << ")." << endl;
	cout<<"fin Parcours::Parcours()"<<endl;
}

void Parcours::regenerateFicCfg(int err) {
    if(err!=1)
        cout << "Régénération fichier config.cfg..." << endl;
    else cout << "Récupération des fichiers en cas d'erreur..." << endl;
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
            cout << "    Scan des fichiers cachés activés" << endl;
		}
		config.close();
	}
    else cout << "    Erreur ouverture fichier config (" << cheminFicCfg << ")." << endl;
    if(err!=1)
        cout << "    config.cfg régénéré."<<endl;
    else cout << "    Récupération prête..." << endl;
}
