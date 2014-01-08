#ifndef THREAD_H
#define THREAD_H
#include <QtCore>

class MainWindow;


class Thread : public QThread
{
public:
    Thread();
	void run();
signals:
	void scanFinished();
};

#endif // THREAD_H
