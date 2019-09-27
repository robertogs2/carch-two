#include "master.h"

/*! Instanciates a new master.
*/

Master::Master(){
    cv_ = new std::condition_variable();
    cv_m_ = new std::mutex();
    clock_ = false;

    //Creates the processors
    for(size_t i = 0; i < PROCESSORS_AMOUNT; ++i){
        std::ostringstream out;
        out << "CPU";
        out << i;
        processors_[i] = new Processor(out.str(), i, cv_, &clock_);
    }

    //Assigns bus controller to cache controllers
    bus_controller_ = new BusController(processors_,  cv_, &clock_);
    for(size_t i = 0; i < PROCESSORS_AMOUNT; ++i){
        processors_[i]->cache_controller_->setBusController(bus_controller_);
    }

    //Assign cache controllers and bus
    coherency_bus_ = new CoherencyBus(processors_, bus_controller_);

}

/*! Starts the threads for the system, for each processor and for the bus controller.
*/

void Master::run(){

    //Start threads
    for(size_t i = 0; i < PROCESSORS_AMOUNT; ++i){
        processor_threads_[i] = new std::thread (&Processor::loop, processors_[i]);
    }
    bus_controller_thread_ = new std::thread (&BusController::loop,  bus_controller_);

    //Start clock
    while(true){
        if(GUI::active){ // Check if user wants it to run

            // negedge
            clock_ = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(Constants::clock_half_period_ms));
            cv_->notify_all(); //broadcast

            // posedge
            clock_ = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(Constants::clock_half_period_ms));
            cv_->notify_all(); //broadcast

            // cycle count
            ticksPassed_++;
        }
    }
}

/*! Updates the Bernoulli probability for a given processor.
    \param id The id of the processor, -1 for all
    \param p1 The probability for processing type
    \param p1 The probability for reading type
    \param p1 The probability for writing type
*/

void Master::updateInstructionProbability(int id, float p1, float p2, float p3){
    processors_[id]->setInstructionProbability(p1, p2, p3);
}

/*! Updates the binomial probability for a given processor.
    \param id The id of the processor, -1 for all
    \param p1 The median scaled from 0 to 1
*/

void Master::updateAddressProbability(int id, float p1){
    processors_[id]->setMemoryProbability(p1);
}
