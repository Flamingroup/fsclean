#include "thread.h"
#include <QtCore>
#include "parcours.h"

Thread::Thread()
{
}

void Thread::run()
{
	Parcours *p=Parcours::getInstance();
    p->regenerateFicCfg(1);
    cout<<"Début scan..."<<endl;
    if(!Parcours::STOP)
        p->runAll();
    cout<<"Fin scan."<<endl;
    emit scanFinished();
}
