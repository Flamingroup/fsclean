#ifndef SQL_H
#define SQL_H
#include <QSqlDatabase>
#include "fichier.h"
#include <QSqlQueryModel>
#include <QThread>
#include <QMutex>

class Sql {
private :
        /**
         * @brief db : Base de donnée du programme
         */
		QSqlDatabase db;
        /**
         * @brief _instance : Instance de la classe SQL : DP singleton
         */
		static Sql* _instance;
        /**
         * @brief Sql constructeur de SQL
         * @param p path
         */
        Sql(path* p=new path("fsclean.db"));
        /**
         * @brief mutex : Mutex d'accès à la base de données
         */
        QMutex mutex;
public:
        /**
         * @brief strings : Requetes effectuées par sqlSelectDoublons()
         */
        string MD5 = "SELECT chemin, filenametrime, poids, datemodif, MD5 FROM Fichiers WHERE(MD5 IN(SELECT MD5 FROM Fichiers WHERE 1 GROUP BY MD5 HAVING COUNT(MD5)>1))ORDER BY MD5,chemin";
        string filenametrime = "SELECT chemin, filenametrime, poids, datemodif FROM Fichiers WHERE filenametrime IN (SELECT filenametrime FROM Fichiers WHERE 1 GROUP BY filenametrime HAVING COUNT(filenametrime)>1) ORDER BY filenametrime,chemin";
        string dossier = "SELECT chemin, nbfic FROM Dossiers WHERE isdoublon = 1 AND nbfic > 0 AND nbfic IN (SELECT nbfic FROM Dossiers WHERE 1 GROUP BY nbfic HAVING COUNT(nbfic)>1) ORDER BY nbfic DESC, chemin";
        /**
         * @brief getInstance : Récupère l'instance de Sql et la créé si besoin
         * @return pointeur sur la classe Sql
         */
        static Sql* getInstance();
        /**
         * @brief sqlInsert : Insère un fichier dans la BDD
         * @return OK si aucun probleme ou ERR si une erreur est survenue
         */
        bool sqlInsert(const Fichier &);
        /**
         * @brief sqlInsertDossier : Insère un dossier dans la table de dossier
         * @param str chgemin du dossier
         * @return false en cas d'erreur
         */
        bool sqlInsertDossier(const string& str);
        /**
         * @brief sqlDelDeletedFiles : Supprime de la base de donnée les fichiers qui n'existent pas sur le disque
         * @return false en cas d'erreur avec la bdd
         */
        bool sqlDelDeletedFiles();
        /**
         * @brief sqlRaz : Reset les flags d'existance des fichiers dans la base de donnée
         *                 Flag sur lequel se base sqlDelDeletedFiles pour supprimer
         * @return
         */
        bool sqlRaz();
        /**
         * @brief Affiche la base de donnée toute entière
         * @return false en cas d'erreur avec la bdd
         */
        bool Affiche();
        /**
         * @brief sqlCreateMD5 : Sélectionne les fichiers dont le poids apparait plus d'une fois en BDD
         *                       Et appel sqlSetMd5() pour mettre les md5 dans la base de donnée
         * @return false en cas d'erreur avec la bdd
         */
        bool sqlCreateMD5();
        /**
         * @brief sqlDelete : Supprime un fichier de la base de donnée
         * @param chemin : chemin du fichier à supprimer
         * @return false en cas d'erreur avec la bdd
         */
		bool sqlDelete(string chemin);
        /**
         * @brief sqlSetMd5 : Update un fichier en BDD avec le md5 calculé
         * @param f fichier à update
         * @return false en cas d'erreur avec la bdd
         */
		bool sqlSetMd5(Fichier &f);
        /**
         * @brief isDossierDoublon Vérifie si un dossier est potentielement un doublon
         * @param chemin : chemin du dossier à vérifier
         * @return true si le dossier est un doublon potentiel
         */
		bool isDossierDoublon(const string& chemin);
        /**
         * @brief sqlSetDossierDoublons : Set le flag d'existance des duoblons dans la base de donnée
         *                                la ou isDossierDoublon() renvoie true;
         * @return false en cas d'erreur avec la bdd
         */
		bool sqlSetDossierDoublons();
        /**
         * @brief sqlSelectDoublons : select les doublons en base de donnée pour les afficher dans l'interface graphique
         * @param select : string de la requete à effectuer (strings en début de classe
         * @return NULL en cas de problème ou un pointeur sur un qsqlqueryModel
         */
        QSqlQueryModel *sqlSelectDoublons(string select);
};

#endif // SQL_H
