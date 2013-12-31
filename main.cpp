#include <iostream>
#include <boost/filesystem.hpp>
#include "parcours.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

using namespace boost::filesystem;
using namespace std;

int main(int argc, char *argv[])
{
    /**
     *Créer le find
     *l'initialiser
     *l'execute dans un fork avec redirection du tube
     *le programe pere lit l'entrée standard
     *sur chaque chemin lu, il créé un nouveau fichier et le remblit grace à boost ou stst
     *l'insertt en bdd
     *le détruit
     *
	 *
	 *si le fichier de config du find existe sinon le créer par défaut
	 *		sinon le créer , le remplis avec les valeurs par défaut
	 *		le load comme config du find
    */
    cout<<"Creation parcours"<<endl;
    Parcours parc;
    cout<<"runall sur parcours"<<endl;
    parc.runAll();
    cout<<"test whitelist"<<endl;
    parc.voirWL();
    cout<<"test blacklist"<<endl;
    parc.voirBL();
    cout<<"fin main"<<endl;
    //tester avec test dans ./a dossier pour vérifier taille dossier
    return 0;
}

/* Version avec le find non portable
 *
 *
 *	path p="./.config.cfg";
	if (!exists(p)){
		// makeFicCfg();
	}
	if (!is_regular_file(p)){
		std::cout << "Erreur lors de l'ouverture du fichier de config" << std::endl;
		return 1;
	}
	findbash f();
	string cmdfind=f.getFind();
	int tube[2];
	if (pipe(tube) == -1){
		perror("Erreur création du tube");
		return 1;
	}
	pid_t pid;
	switch (pid=fork()){
		case -1: std::cout << "Erreur lors du fork" << std::endl;
			return -1;
		case 0: //code fils
			close(tube[0]);
			dup2(tube[1], 1);
			close(tube[1]);
			execl("/usr/bin/find", "find", "/home/etud/info", "-maxdepth 1", "-name", "alnoel2", (char*)NULL);
			return 0;
		default: break;
	}
	//code pere
	close(tube[1]);
	dup2(tube[0], 0);
	close(tube[0]);

	string retFromFils;
	while (cin >> retFromFils) {
		cout << retFromFils << endl;
		retFromFils.clear();
	}
	wait(NULL);
*/
