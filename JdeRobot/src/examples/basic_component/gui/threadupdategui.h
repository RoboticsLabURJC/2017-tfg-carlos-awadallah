#ifndef THREADUPDATEGUI_H
#define THREADUPDATEGUI_H

#include "gui.h"
#include <QtWidgets>
#include <iostream>
#include <sys/time.h>

#define cycle_update_gui 50 //miliseconds

class ThreadUpdateGUI: public QThread
{
public:
    ThreadUpdateGUI(Comm::Communicator* jdrc, Comm::CameraClient* camera, Comm::MotorsClient* motors);

private:
    GUI* gui;

protected:
    void run();
};

#endif // THREADUPDATEGUI_H