#ifndef THREAD_H
#define THREAD_H
#include <QtCore>

class MainWindow;


class Thread : public QThread
{
    Q_OBJECT
public:
    /**
     * @brief Thread : classe héritée de Qthread surchargeant son run()
     */
    Thread();
    /**
     * @brief run : lance le scan dans un thread, stoppable par le bouton arret scan dans GUI
     */
	void run();
signals:
    /**
     * @brief scanFinished : signal émis lorsque le thread a terminé le scan
     */
    void scanFinished();
};

#endif // THREAD_H
