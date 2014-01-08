#include "thread.h"
#include <QtCore>
#include "parcours.h"

Thread::Thread()
{
}

void Thread::run()
{
    cout<<"debut thread"<<endl;
    //Parcours p;
    //p.runAll();
    cout<<"fin thread"<<endl;
}
