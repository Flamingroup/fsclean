#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "about.h"
#include <QMainWindow>
#include "thread.h"
#include "parcours.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief MainWindow : fenetre principale du programme
     * @param parent : toujours nul, MainWindow est la fenetre principale
     */
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    /**
     * @brief displayInStatusBar : affiche un message dans la barre de statut
     * @param message : string contenant le message à afficher
     */
    void displayInStatusBar(const std::string & message);
    /**
     * @brief displayNbElemBDDInStatusBar : affiche le nombre de fichiers scannables dans
     * la barre de statut
     */
	void displayNbElemBDDInStatusBar();
    /**
     * @brief scan : Classe héritée de QThread surchargeant la méthode run() pour faire un scan
     */
    Thread * scan;
    /**
     * @brief timer : timer servant à régulièrement maj la barre de progression (10par s)
     */
	QTimer *timer;
public slots:
    /**
     * @brief setProgress : capte le signal du timer pour changer la valeur de la barre de progression
     */
	void setProgress();
    /**
     * @brief FinScan : capte le signal scanFinished de Thread et maj l'affichage
     */
    void FinScan();
    /**
     * @brief bloquerSuppr : au changement d'onglet cette méthode désactive ou réactive
     *le bouton de suppression, qui est désactivé pour les dossiers
     * @param index : numéro de l'onglet, commence à 0
     */
    void bloquerSuppr(int index);
private slots:
    /**
     * @brief setLEDGreen : "rallume" la petite led de la GUI en vert
     */
    void setLEDGreen();
    /**
     * @brief setLEDRed : colore la petite led de la GUI en rouge
     */
    void setLEDRed();
    /**
     * @brief on_scanButton_clicked : lance le thread qui s'occupe du scan, désactive une partie
     * de l'affichage pour éviter les changements des listes en cours de scan.
     */
    void on_scanButton_clicked();
    /**
     * @brief remplirWL : remplit la fenetre liste blanche avec le contenu de la map
     */
	void remplirWL();
    /**
     * @brief remplirBL : remplit la fenetre liste noire avec le contenu de la map
     */
	void remplirBL();
    /**
     * @brief on_lessWLButton_clicked : enlève un élément de liste blanche en GUI et fait la
     * même chose avec la map liste blanche
     */
    void on_lessWLButton_clicked();
    /**
     * @brief on_lessBLButton_clicked : enlève un élément de liste noire en GUI et fait la
     * même chose avec la map liste noire
     */
    void on_lessBLButton_clicked();
    /**
     * @brief on_actionReinitialiser_param_defaut_triggered : recrée une liste blanche et noire par défaut
     */
    void on_actionReinitialiser_param_defaut_triggered();
    /**
     * @brief on_WLplusButton_clicked : ajoute un élément de liste blanche en GUI et fait la
     * même chose avec la map liste blanche
     */
    void on_WLplusButton_clicked();
    /**
     * @brief on_plusBLButton_clicked : ajoute un élément de liste noire en GUI et fait la
     * même chose avec la map liste noire
     */
    void on_plusBLButton_clicked();
    /**
     * @brief on_actionA_propos_triggered : affiche un message d'information TRES utile sur le programme
     * et ses développeurs
     */
    void on_actionA_propos_triggered();


	void on_Buttonrafraichir_clicked();

    void on_Buttonsupprimer_clicked();

    void on_Buttonmasquer_clicked();

    void on_quitButton_clicked();

private:
    Ui::MainWindow *ui;
	About *popup;
};

#endif // MAINWINDOW_H
