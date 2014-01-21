#ifndef FICHIER_H
#define FICHIER_H
#include <QSqlQuery>
#include <QString>
#include <QVariant>
#include <boost/filesystem.hpp>
using namespace boost::filesystem;

using namespace std;
/**
 * @brief The Fichier class : used to store informations about a file
 */
class Fichier
{
	private:
        /**
         * @brief id id du fichier en base de donnée, initialisé à -1
         */
		int id;
        /**
         * @brief chemin : boost::path : contient les informations d'un fichier
         */
		path chemin;
        /**
         * @brief filenameTrime : Nom du fichier sans accents ni caractères spéciaux.
         */
		string filenameTrime;
        /**
         * @brief poids : poids du fichier ssur 64 bits pour gérer les gros systemes de fichiers
         */
		u_int64_t poids;
        /**
         * @brief dateModif : Date de derniere modification du fichier
         */
		u_int32_t dateModif;
        /**
         * @brief MD5 : Contient le MD5 du fichier
         */
        string MD5;
	public:
        /**
         * @brief Constructeur sans argument, utilisé avec Fichier::remplir() pour le DP de la pool
         */
		Fichier();
        /**
         * @brief Fichier : Constructeur de Fichier à partir d'une qsqlquery, suite à une sélection dans la base de donnée
         */
		Fichier(QSqlQuery&);

        /**
         * @brief Getteurs / Setteurs
         */
        int getid() const;
        void setid(int i);

		const path& getChemin() const;

        const string& getfilenameTrime() const;
        void setfilenameTrime(string s);

        const u_int64_t& getPoids() const;
        void setPoids(u_int64_t pds);

		const u_int32_t& getDateModif() const;

		const string& getMD5() const;

        /**
         * @brief remplir : vide le fichier et le remplit à partir des informations conenues dans le path
         * @param p : path de boost à partir duquel on récupère les informations
         */
		void remplir(path p);
        /**
         * @brief trim : Supprime les accents et caractères spéciaux du nom d'un fichier
         * @param s : string à nettoyer
         * @return string : nom du fichier sans accent ni caractères spéciaux
         */
		string trim(string s);
        /**
         * @brief voir : Affiche les informations d'un fichier dans le terminal
         */
        void voir();
        /**
         * @brief calcMD5 : Créé le md5 du fichier grâce à libtomcrypt (md5 tout pas beau)
         */
        void calcMD5();
};
#endif // FICHIER_H

/* TODO : tester dans un log :
 * enumeration correcte des éléments parcourus avec le find
 * remplir un fichier et l'afficher
 * recherche des doublons et stockage dans un log (chiant de parcourir un fichier texte)
*/
