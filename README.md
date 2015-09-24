fsclean
=======

=== Noms/prénoms du binôme ===
Bouterige Raphael
Daval Loic
== Parcours ==
* parcourt des arborescences de fichier
OUI
* calcule des clefs de hachage (MD5, SHA...)
OUI
* stocke le résultat du parcours en base de donnée
OUI
* permet de ne reparcourir que ce qui a déjà été modifié
OUI
* a des fonctionnalités de parcours non évoquées précédemment
Possibilité de mettre des dossiers sur liste noire
== Recherche des doublons ==
* trouve les fichiers de même nom
OUI
* trouve les fichiers de noms proches
OUI
* trouve tous les doublons (contenu) présents (et seulement eux)
OUI
* sait trouver des répertoires identiques
OUI
* sait trouver des arborescences identiques
NON
* a des fonctionnalités de recherche non évoquées précédemment
présent dans le code mais non utilisé : le scan ou non des fichiers cachés
== Affichage des résultats et interaction ==
* a une interface en Qt toute jolie
OUI
* a une interface texte pratique
OUI MAIS : pas interactive.
* peut reconstruire la base automatiquement
OUI
* affiche élégamment la liste des doublons
OUI
* permet d'ordonnancer les doublons
NON : déjà ordonnés, mais on peut les masquer/révéler de la liste
* permet de supprimer les doublons
OUI
* avec sélection multiple manuel
OUI
* avec sélection automatique
NON
* peut enregistrer des options (choix des zones indexées)
OUI
* a une interface agréable a utiliser
OUIIIIIIII
== Autre ==
Mon programme a d'autres fonctionnalités, que je liste ici:
*Le menu option permet de reset le fichier de config et donc de récupérer des paramètres par défaut.
*Le scan est fait par un thread, permettant à l'utilisateur de garder la main sur la GUI.
*L'utilisateur peut arreter un scan.
*En cas d'arret brutal du programme, celui-ci récupère au lancement suivant.
*En cas de suppression par l'utilisateur, le fichier config.cfg est régénéré.
== Bibliothèques utilisées ==
Listez ici les bibliothèques utilisées. Si vous avez récupéré des bouts de
code d'ailleurs, précisez lesquels et leur provenance.
*QT
*Boost
*libTomCrypt
