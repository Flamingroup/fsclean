#include <iostream>
#include <boost/filesystem.hpp>
#include "parcours.h"
#include "sql.h"
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
//	path p (argv[1]);
//	Sql* mabase=Sql::getInstance();
//	mabase->sqlRaz();
//	Fichier f;
//	f.remplir(p);
//	mabase->sqlInsert(f);
//	mabase->sqlDelDeletedFiles();
//	mabase->Affiche();

	Parcours parc;
	Sql* mabase=Sql::getInstance();
	parc.runAll();
	parc.voirWL();
	parc.voirBL();
	mabase->Affiche();
	//tester avec test dans ./a dossier pour vérifier taille dossier
	return 0;
}

//path p (argv[1]);   // p reads clearer than argv[1] in the following code

//	try
//	{
//		if (exists(p))    // does p actually exist?
//		{
//			if (is_regular_file(p))        // is p a regular file?
//				cout << p << " size is " << file_size(p) << '\n';

//			else if (is_directory(p))      // is p a directory?
//			{
//				cout << p << " is a directory containing:\n";
//				directory_iterator it(p);
//				directory_iterator end;
//				for (; it != end; ++it){
//					cout<< *it << " : " << endl;
//				}
//				cout << endl;
//				copy(directory_iterator(p), directory_iterator(), // directory_iterator::value_type
//					 ostream_iterator<directory_entry>(cout, "\n")); // is directory_entry, which is
//				// converted to a path by the
//				// path stream inserter
//			}

//			else
//				cout << p << " exists, but is neither a regular file nor a directory\n";
//		}
//		else
//			cout << p << " does not exist\n";
//	}

//	catch (const filesystem_error& ex)
//	{
//		cout << ex.what() << '\n';
//	}

//	return 0;
//}


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
