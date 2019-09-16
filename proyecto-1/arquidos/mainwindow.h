#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QObject>
#include "ui_mainwindow.h"
#include "gui.h"
#include "master.h"
class Master;
namespace Ui {
class MainWindow;
}

/*!
    \class MainWindow
    \brief The MainWindows class implements a basic Qt window, aswell
    as methods to update it.
*/

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow* getUI();
    void setMaster(Master* master);
    void startWorking();
    ~MainWindow();

    void loopUpdater();

    QTextEdit* getCPU(size_t index);

    QTextEdit* getCache(size_t index);

    QTextEdit* getControl(size_t index);

    QTextEdit* getMemory();

    QTextEdit* getBus();

    QTextEdit* getBusController();

    Master* master_;



public slots:
    void slotUpdateGUI();

private slots:
    void on_activebutton_clicked();
    void on_periodButton_clicked();

    void on_instructionButton_clicked();

    void on_addressButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
