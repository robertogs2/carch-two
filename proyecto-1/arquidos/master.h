#ifndef MASTER_H
#define MASTER_H

#include <string>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <sstream>

#include "processor.h"
#include "buscontroller.h"
#include "coherencybus.h"

/*!
    \class Master
    \brief The Master class implements a basic controller for a
    multiprocessor system, instanciating the processors, bus controller,
    coherency bus, and others. Also it implements the basic clock signal for
    all the system.

*/

class Master{
public:

    // clock variables
    std::condition_variable* cv_;
    std::mutex* cv_m_;
    bool clock_;

    // instances
    Processor* processors_[PROCESSORS_AMOUNT];
    BusController* bus_controller_;
    CoherencyBus* coherency_bus_;

    // threads
    std::thread* processor_threads_[PROCESSORS_AMOUNT];
    std::thread* bus_controller_thread_;

    // gui
    int ticksPassed_ = 0;


    /*! Instanciates a new master.
    */
    Master();

    /*! Starts the threads for the system, for each processor and for the bus controller.
        \param counts The A/D counts to convert.`
        \return The calculated kg based on the parameter.
    */
    void run();

    /*! Updates the Bernoulli probability for a given processor.
        \param id The id of the processor, -1 for all
        \param p1 The probability for processing type
        \param p1 The probability for reading type
        \param p1 The probability for writing type
    */
    void updateInstructionProbability(int id, float p1, float p2, float p3);

    /*! Updates the binomial probability for a given processor.
        \param id The id of the processor, -1 for all
        \param p1 The median scaled from 0 to 1
    */
    void updateAddressProbability(int id, float p1);

};

#endif // MASTER_H
