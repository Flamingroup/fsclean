#ifndef PARCOURS_H
#define PARCOURS_H

#include <iostream>
#include <map>
using namespace std;

#include <boost/filesystem.hpp>
using namespace boost::filesystem;

class Parcours
{
	private:

		map<string, path*> listeblanche;
		/**
		 * @brief listenoire : liste des sous dossiers de liste blanche à ne pas scanner
		 */
		map<string, path*> listenoire;
	public:
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
		path * stringToPath(string toTransform);
        /**
         * @brief inTheFind
         * @return return true if chemin will be scanned
		 */
		void addToWL(string chemin);
		void addToBL(string chemin);
		void rmvFromWL(string chemin);
		void rmvFromBL(string chemin);
		/**
		 * @brief executer
		 */
		void executer();
};

#endif // FIND_H
