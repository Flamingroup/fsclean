#ifndef SQL_H
#define SQL_H
#include "fichier.h"

class Sql
{
public:
    static bool sqlInsert(Fichier);
    static bool sqlDelete(Fichier);
    static bool sqlUpdate(Fichier);
};

#endif // SQL_H
