#include "parcours.h"
#include "fichier.h"
#include "sql.h"
#include <iostream>
#include <string>
#include <fstream>

Parcours::Parcours(string chemin)
{
    cout<<"Parcours::Parcours()\n"<<endl;
    //cout<<chemin<<endl;
    ifstream config(chemin.c_str(), ios_base::in);
    if (config) {
		string ligne;
        char mode=0;
        // 0 : on attends de trouver liste blanche
        // 1: on insère dans liste blanche
        // 2: on insère dans liste noire
        //cout<<"debut while : "<<endl;
		while (getline(config, ligne)) {
            //cout<<"test2"<<endl;
            //cout<<"ligne="<<ligne<<endl;
			if (mode == 0 && ligne != "listeblanche")
				continue;
			if (mode == 0 && ligne == "listeblanche"){
				mode=1;
				continue;
			}
			if (mode == 1 && ligne != "listenoire") {
                cout<<"listeBlanche mise à jour"<<endl;
				addToWL(ligne);
			}
			if (mode == 1 && ligne == "listenoire") {
				mode=2;
				continue;
			}
			if (mode == 2 ) {
                cout<<"listeNoire mise à jour"<<endl;
                addToBL(ligne);
			}
		}
        //cout<<"fin while"<<endl;
		config.close();
	}
    //else cout<<"config!"<<endl;
    cout<<"fin Parcours::Parcours()"<<endl;
}

path* Parcours::stringToPath(string toTransform)
{
	if (toTransform[0] == '~' || !exists(toTransform))
		return 0;
	path *p=new path(toTransform);
	*p=canonical(*p);
    return p;
}

void Parcours::voirWL()
{
    cout<<"contenu Wlist : "<<endl;
    map<string, path*>::iterator it = listeblanche.begin();
    map<string, path*>::iterator fin = listeblanche.end();
    for(;it!=fin;it++){
        cout<<"string="<<(*it).first<<endl<<"path="<<((*it).second)->string()<<endl;
    }
}

void Parcours::voirBL()
{
    cout<<"contenu Blist : "<<endl;
    map<string, path*>::iterator it = listenoire.begin();
    map<string, path*>::iterator fin = listenoire.end();
    for(;it!=fin;it++){
        cout<<"string="<<(*it).first<<endl<<"path="<<((*it).second)->string()<<endl;
    }
}

void Parcours::addToWL(string chemin)
{
    ///attention : le path ne prend pas la fin du chemin!
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "addToWL : path tmp=" << tmp->string() <<endl;
	if (listeblanche.find(tmp->string()) == listeblanche.end()) {
		listeblanche[tmp->string()]=tmp;
        cout<<"ajout fait"<<endl;
    }
    else cout<<"deja dans WL"<<endl;
}

void Parcours::addToBL(string chemin)
{
    path *tmp = stringToPath(chemin);
	if (tmp == NULL) return;
    cout << "addToBL : path tmp=" << tmp->string() <<endl;
	if (listenoire.find(tmp->string()) == listenoire.end()) {
		listenoire[tmp->string()]=tmp;
        cout<<"ajout fait"<<endl;
    }
    else cout<<"deja dans BL"<<endl;
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
     * Sinon, c'est un dossier
	 *		on l'ajouet à la pile si il est pas dans la liste noire
	 * On recommence jusqu'a pile vide
	 *
	 */
	map<string, path*>::iterator it=listeblanche.begin();
	map<string, path*>::iterator end=listeblanche.end();
	for(; it!=end; ++it){
        cout<<"Parcours::runall() : "<<(*it).first<<endl<<" va etre inspecté"<<endl;
		runFromPath(*it);
	}
}

void Parcours::runFromPath(const pair<string, path*>& thePair)
{
	list<path*> directories;
    directories.push_back(new path(*thePair.second));
	Fichier f;
	try{
        while(!directories.empty()){
            if(exists(*directories.front())){
                directory_iterator it(*directories.front());
                directory_iterator end;
                for (; it != end; ++it){
					if(is_regular_file(*it) && !isInBlacklist(it->path())) {
                        f.remplir(it->path());
                        Sql::sqlInsert(f);
                    }
					else if(is_directory(*it) && !isInBlacklist(it->path())) {
                        directories.push_back(new path(it->path()));
                    }
					else cout << directories.front()->string() << "exists, but not regular file nor directory or is in blacklist." << endl;
                }
            }
        else cout << directories.front()->string() << "does not exist anymore" << endl;
        delete directories.front();
        directories.pop_front();
        }
    }
    catch (const filesystem_error& ex)
    {
        cout << ex.what() << '\n';
    }
}

bool Parcours::isInBlacklist(const path & p)
{
	if (listenoire.find(p.string()) == listenoire.end())
		return false;
	return true;
}


