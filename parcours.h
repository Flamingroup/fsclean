#ifndef PARCOURS_H
#define PARCOURS_H

#include <QSqlQueryModel>
#include <iostream>
#include <map>
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

class Parcours
{
    private:
    string cheminFicCfg;
    public:
		/**
         * @brief listeblanche : liste des dossiers à scanner*/
         // todolisteblanche devrait etre une liste ce serait mieux
		map<string, path*> listeblanche;
		/**
		 * @brief listenoire : liste des sous dossiers de liste blanche à ne pas scanner
		 */
		map<string, path*> listenoire;
    //public:
		/**
		 * @brief Parcours
		 * @param chemin : chemin du fichier contenant les paramètres de recherche des doublons
		 */
		Parcours(string chemin="./config.cfg");
		/**
		 * @brief stringToPath : Transforme nimporte quelle string de chemin relatif en path avec chemin absolu
		 * @param toTransform : chemin du fichier à transformer
		 * @return
		 */
        static path* stringToPath(string toTransform, bool verifExist=true);
        void voirWL();
        void voirBL();
		void addToWL(string chemin);
		void addToBL(string chemin);
		void rmvFromWL(string chemin);
		void rmvFromBL(string chemin);
		/**
		 * @brief executer : Lance le parcours et insert les fichiers dans la BDD au fur et à mesure.
		 */
		void runAll();
		void runFromPath(const pair<string, path *> & thePair);
        bool isInBlacklist(const path &p);
		bool isHidden(const path & p);
        void resetFicCfg();
        void regenerateFicCfg();
        //default behavior of sqlSelect : searching for duplicates
        QSqlQueryModel *sqlSelect(string requete = "SELECT * FROM Fichiers WHERE MD5 IN (SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1)");
};

#endif // FIND_H
