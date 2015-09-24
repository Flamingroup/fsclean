fsclean
=======

=== Noms/prénoms du binôme ===
Bouterige Raphael
Daval Loic
== Parcours ==
* parcourt des arborescences de fichier
* calcule des clefs de hachage (MD5, SHA...)
* stocke le résultat du parcours en base de donnée
* permet de ne reparcourir que ce qui a déjà été modifié
* a des fonctionnalités de parcours non évoquées précédemment
* Possibilité de mettre des dossiers sur liste noire


== Recherche des doublons ==
* trouve les fichiers de même nom
* trouve les fichiers de noms proches
* trouve tous les doublons (contenu) présents (et seulement eux)
* sait trouver des répertoires identiques
* sait trouver des arborescences identiques
* a des fonctionnalités de recherche non évoquées précédemment
* présent dans le code mais non utilisé : le scan ou non des fichiers cachés

== Affichage des résultats et interaction ==
* a une interface en Qt toute jolie
* a une interface texte pratique
* MAIS : pas interactive.
* peut reconstruire la base automatiquement
* affiche élégamment la liste des doublons
* Ne permet pas d'ordonnancer les doublons : déjà ordonnés, mais on peut les masquer/révéler de la liste
* permet de supprimer les doublons
* avec sélection multiple manuel
* Pas de sélection automatique
* peut enregistrer des options (choix des zones indexées)
* a une interface agréable a utiliser

== Autre ==
Mon programme a d'autres fonctionnalités, que je liste ici:
* Le menu option permet de reset le fichier de config et donc de récupérer des paramètres par défaut.
* Le scan est fait par un thread, permettant à l'utilisateur de garder la main sur la GUI.
* L'utilisateur peut arreter un scan.
* En cas d'arret brutal du programme, celui-ci récupère au lancement suivant.
* En cas de suppression par l'utilisateur, le fichier config.cfg est régénéré.

== Bibliothèques utilisées ==
Listez ici les bibliothèques utilisées. Si vous avez récupéré des bouts de
code d'ailleurs, précisez lesquels et leur provenance.
* QT
* Boost
* libTomCrypt
