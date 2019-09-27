#include "mainwindow.h"
#include "master.h"
#include "ui_mainwindow.h" //To use ui
#include "gui.h"
#include <QApplication>
#include <random>
#include <time.h>

int main(int argc, char *argv[]){
    QApplication a(argc, argv);
    MainWindow w;

    // Initializes all resources
    Master* master = new Master();
    w.setMaster(master);

    // Runs application but waits for toggle
    new std::thread (&Master::run,  master);

    // Starts thread for gui updating
    w.show();
    w.loopUpdater();

    //default return
    return a.exec();
}

