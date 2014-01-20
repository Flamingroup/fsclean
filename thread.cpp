#include "thread.h"
#include <QtCore>
#include "parcours.h"

Thread::Thread()
{
}

void Thread::run()
{
    cout<<"Début scan..."<<endl;
	Parcours *p=Parcours::getInstance();
    p->regenerateFicCfg(1);
	p->runAll();
    p->regenerateFicCfg();
    cout<<"Fin scan."<<endl;
    emit scanFinished();
}
