#include "sql.h"

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <fstream>
#include <sstream>

bool Sql::sqlInsert(Fichier f)
{
    //todo : insertion d'un fichier rempli dans la bdd
    /// partie suppléant la bdd pour test dans un log
    ///int log;
    ///if((log = open("./log", O_APPEND | O_CREAT)) == -1)
    ///    perror("open log"), exit(1);
    ///
    ostringstream sortie;
    sortie <<f.getid()<<";"<<f.getPath()<<";"<<f.getMD5()<<";"<<f.getfilenameTrime()<<";"<<f.getPoids()<<";"<<f.getDateModif() << endl;
    string ligne = sortie.str();
    cout<<sortie;//"ligne="<<ligne<<endl<<"sizeofligne="<<sizeof(ligne)<<endl;
    /*
    if(!write(log, &ligne, sizeof(ligne)))
        perror("write log"), exit(1);
    close(log);
    */
    return true;
}

bool Sql::sqlDelete(Fichier)
{
    //todo : suppression d'un fichier de la bdd
    return false;
}

bool Sql::sqlUpdate(Fichier)
{
    //todo : changement des caracs d'un fichier de la bdd (MD5 surtout)
    return false;
}
