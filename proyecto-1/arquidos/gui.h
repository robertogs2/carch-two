#ifndef GUI_H
#define GUI_H
#include <QThread>
#include <QObject>
#include "mainwindow.h"
#include "constants.h"
#include <iostream>

/*!
    \class GUI
    \brief The GUI class defines the interface to update the
    Qt gui with the system data

    GUI uses a set of static variables that can be changed to change
    the displayed content in the screen. GUI is a QThread that uses
    signals and slots to communicate with the GUI thread.
*/

class GUI : public QThread{
    Q_OBJECT
public:


    inline static std::string processor_states[PROCESSORS_AMOUNT];
    inline static std::string cache_states[PROCESSORS_AMOUNT];
    inline static std::string control_states[PROCESSORS_AMOUNT];
    inline static std::string memory_state;
    inline static std::string bus_state;
    inline static std::string bus_controller_state;
    inline static bool change;
    inline static bool active;
    inline static bool changeActive;

    void run(){
        while(true){
            if(change){
                emit signalUpdate();
                change=false;
            }
            if(changeActive){
                active=!active;
                changeActive=false;
            }
        }
    }

signals:
    void signalUpdate();
};

#endif // GUI_H

