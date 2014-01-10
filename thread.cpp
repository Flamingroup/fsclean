#include "thread.h"
#include <QtCore>
#include "parcours.h"

Thread::Thread()
{
}

void Thread::run()
{
    cout<<"debut thread"<<endl;
	Parcours *p=Parcours::getInstance();
	p->runAll();
	cout<<"fin thread"<<endl;
    emit scanFinished();
}
