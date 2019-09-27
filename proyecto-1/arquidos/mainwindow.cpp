#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "gui.h"
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow){
    ui->setupUi(this);
}

MainWindow::~MainWindow(){
    delete ui;
}

Ui::MainWindow* MainWindow::getUI(){
    return ui;
}

void MainWindow::setMaster(Master *master){
    master_=master;
}

void MainWindow::slotUpdateGUI(){
    for(size_t i = 0; i < PROCESSORS_AMOUNT; ++i){
        getCPU(i)->setText(QString(GUI::processor_states[i].c_str()));
        getCache(i)->setText(QString(GUI::cache_states[i].c_str()));
        getControl(i)->setText(QString(GUI::control_states[i].c_str()));
        getBus()->setText(QString(GUI::bus_state.c_str()));
        getBusController()->setText(QString(GUI::bus_controller_state.c_str()));
        getMemory()->setText(QString(GUI::memory_state.c_str()));
        ui->ticksLabel->setText(QString("Ticks passed: ") + QString::number(master_->ticksPassed_));
    }
}

void MainWindow::loopUpdater(){
    QThread *thread = new QThread(this);
    GUI *gui = new GUI();

    gui->moveToThread(thread);

    connect(gui, SIGNAL(signalUpdate()), this, SLOT(slotUpdateGUI()));
    connect(thread, SIGNAL(destroyed()), gui, SLOT(deleteLater()));

    gui->start();
}

void MainWindow::on_activebutton_clicked(){
    GUI::changeActive=true;
}

void MainWindow::on_periodButton_clicked(){
    int newPeriod = ui->inputPeriod->text().toInt();
    if(newPeriod >=50){
        std::cout << "New period: " << newPeriod << "ms" << std::endl;
        Constants::clock_half_period_ms = newPeriod/2;
    }
    else{
        std::cout << "Period should be higher or equal than 50" << std::endl;
    }
}

void MainWindow::on_instructionButton_clicked(){
    int id = ui->inputProbCPU->text().toInt();
    if(id > PROCESSORS_AMOUNT){
        std::cout << id << " exceeds amount of processors which is " << PROCESSORS_AMOUNT << std::endl;
    }
    else{
        float p1 = ui->inputProb1->text().toFloat();
        float p2 = ui->inputProb2->text().toFloat();
        float p3 = ui->inputProb3->text().toFloat();
        if(p1 > 1 || p2 > 1 || p3 > 1) std::cout << "Probabilities should sum one, third is calculated as 1-p1-p2" << std::endl;
        else if(id == -1){
            master_->updateInstructionProbability(0, p1, p2, p3);
            master_->updateInstructionProbability(1, p1, p2, p3);
            master_->updateInstructionProbability(2, p1, p2, p3);
            master_->updateInstructionProbability(3, p1, p2, p3);
        }
        else{
            master_->updateInstructionProbability(id, p1, p2, p3);
            std::cout << "New instruction probability set for CPU" << id << std::endl;
        }
    }
}

void MainWindow::on_addressButton_clicked(){
    int id = ui->inputProbCPU->text().toInt();
    if(id > PROCESSORS_AMOUNT){
        std::cout << id << " exceeds amount of processors which is " << PROCESSORS_AMOUNT << std::endl;
    }
    else{
        float p1 = ui->inputProbAddress->text().toFloat();
        if(p1 > 1) std::cout << "Probability should be between 0 and 1" << std::endl;
        else if(id == -1){
            master_->updateAddressProbability(0, p1);
            master_->updateAddressProbability(1, p1);
            master_->updateAddressProbability(2, p1);
            master_->updateAddressProbability(3, p1);
        }
        else master_->updateAddressProbability(id, p1);
        std::cout << "New address probability set for CPU" << id << std::endl;
    }
}


QTextEdit* MainWindow::getCPU(size_t index){
    switch(index){
        case 0:
            return ui->cpu0;
            break;
        case 1:
            return ui->cpu1;
            break;
        case 2:
            return ui->cpu2;
            break;
        case 3:
            return ui->cpu3;
            break;
        default:
            return nullptr;
            break;
    }
}

QTextEdit* MainWindow::getCache(size_t index){
    switch(index){
        case 0:
            return ui->cache0;
            break;
        case 1:
            return ui->cache1;
            break;
        case 2:
            return ui->cache2;
            break;
        case 3:
            return ui->cache3;
            break;
        default:
            return nullptr;
            break;
    }
}

QTextEdit* MainWindow::getControl(size_t index){
    switch(index){
        case 0:
            return ui->control0;
            break;
        case 1:
            return ui->control1;
            break;
        case 2:
            return ui->control2;
            break;
        case 3:
            return ui->control3;
            break;
        default:
            return nullptr;
            break;
    }
}

QTextEdit* MainWindow::getMemory(){
    return ui->memory;
}

QTextEdit* MainWindow::getBus(){
    return ui->bus;
}

QTextEdit* MainWindow::getBusController(){
    return ui->buscontroller;
}

