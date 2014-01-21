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
        /**
         * @brief cheminFicCfg : chemin du fichier de configuration
         */
		string cheminFicCfg;
        /**
         * @brief scannercaches : booléen indiquant si on scan les fichier cachés ou non
         *       Valeur par défaut : false : on ne scan pas les fichiers cachés
         */
		bool scannercaches=false;
        /**
         * @brief nbApprox : Nombre de fichiers déja scannés
         */
        double nbApprox;
        /**
         * @brief denom : Nombre de fichier à scanner selon les parametres actuels
         */
		double denom;
        /**
         * @brief _instance : Pointeur vers l'instance du Parcours : DP singleton
         */
		static Parcours * _instance;
		/**
		 * @brief Parcours
         * @param chemin : chemin du fichier de configuration
		 */
		Parcours(string chemin);
    public:
        /**
         * @brief getInstance : Créé l'instance de parcours si besoin
         * @param chemin : chemin du fichier de config a utiliser pour créer le parcours
         * @return pointeur sur l'instance de Parcours
         */
		static Parcours* getInstance(string chemin="./config.cfg");
        /**
         * @brief AVANCE : Pourcentage d'avancement du scan
         */
        static char AVANCE;
        /**
         * @brief STOP : booleen qui arrete le scan à tout moment
         */
        static bool STOP;
		/**
         * @brief listeblanche : Chemins des dossiers à scanner
        */
		map<string, path*> listeblanche;
		/**
         * @brief listenoire : Chemins des dossiers qui ne seront pas scannés même si ce sont des sous-dossiers de la liste blanche.
		 */
		map<string, path*> listenoire;
		/**
		 * @brief stringToPath : Transforme nimporte quelle string de chemin relatif en path avec chemin absolu
		 * @param toTransform : chemin du fichier à transformer
         * @param verifExist : si verif est à true, on vérifie l'existance du fichier sinon non
         * @return pointeur sur un path
         */
		static path* stringToPath(string toTransform, bool verifExist=true);
        /**
         * @brief voirWL  : Affiche la liste blanche
         */
        void voirWL();
        /**
         * @brief voirWL  : Affiche la liste noire
         */
        void voirBL();
        /**
         * @brief addToWL : Ajoute un dossier à la liste blanche
         * @param chemin : chemin du dossier à rajouter
         */
		void addToWL(string chemin);
        /**
         * @brief addToWL : Ajoute un dossier à la liste noire
         * @param chemin : chemin du dossier à rajouter
         */
        void addToBL(string chemin);
        /**
         * @brief rmvFromWL : Supprime un dossier de la liste blanche
         * @param chemin : chemin du dossier à supprimer
         */
        void rmvFromWL(string chemin);
        /**
         * @brief rmvFromBL : Supprime un dossier de la liste noire
         * @param chemin : chemin du dossier à supprimer
         */
        void rmvFromBL(string chemin);


        inline const double& getNbApprox(){return nbApprox;}
        /**
         * @brief runAll : Lance le parcours avec insertion dans la BDD
         */
        void runAll();
        /**
         * @brief countApprox : Lance le parcours sans insertion dans la bdd
         *                      Compte simplement le nombre d'élément pour la bare d'avancement
         */
        void countApprox();
        /**
         * @brief runFromPath : lance le scan d'un élément de la listeblanche
         * @param thePair : pair string, path* du dossier à scanner
         * @param countOnly : si countonly est à true, on n'insère pas dans la bdd
         */
		void runFromPath(const pair<string, path *> & thePair, bool countOnly=false);
        /**
         * @brief isInBlacklist : Vérifie si un fichier est dans la liste noire ou non
         * @param p : chemin à tester
         * @return true si le fichier est dans la black list
         */
        bool isInBlacklist(const path &p);
        /**
         * @brief isHidden : Vérifie si un fichier est caché ou non
         * @param p : chemin à tester
         * @return true si le fichier est un fichier caché
         */
		bool isHidden(const path & p);
        /**
         * @brief resetFicCfg : recrée le fichier de config aevc les valeur par défaut
         * @param error valeur de "error" dans le fichier de config
         */
        void resetFicCfg(int8_t error=0);
        /**
         * @brief regenerateFicCfg : recréé le fichier de config avec les parametres actuels
         * @param err : valeur mise à 1 avant le lancement du scan
         *              En cas de fermeture brutale, la barre d'avancement restera cohérente à la prochaine execution
         */
        void regenerateFicCfg(int err=0);

		~Parcours();
};


#endif // FIND_H
