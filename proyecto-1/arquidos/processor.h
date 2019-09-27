#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <condition_variable>
#include <thread>
#include <chrono>

#include "cache.h"
#include "cachecontroller.h"
#include "bernoulli.h"
#include "binomial.h"
#include "gui.h"

class CacheController;

/*!
    \class Processor
    \brief The Processor class simulates a processor for a multiprocessor architecture

    Processor simulates a processor which generates instructions of processing,
    reading memory, and writing memory. This processor has associated a cache with
    its own cache controller. These instructions and its addresses are generated
    using Bernoulli and Binomial distributions, respectively. In each of them,
    the cache controller needs to make use of protocols and the bus. The processor
    is controller by a master clock.
*/

class Processor{
public:

    //GUI
    std::string tag_;
    int id_;
    std::string current_state_;

    //Synchronization variables
    std::condition_variable* cv_;
    std::mutex* cv_m_;
    bool* clock_;

    // Modules
    Bernoulli* instruction_bernoulli_;
    Binomial* memory_binomial_;
    Cache* cache_;
    CacheController* cache_controller_;

    // Condition variables
    bool* waiting_memory_;
    int instruction_count_;

    Processor(std::string tag, int id, std::condition_variable* cv, bool* clock);
    void setInstructionProbability(float p1, float p2, float p3);
    void setMemoryProbability(float p);
    size_t generateInstructionType();
    size_t generateMemoryAddress();
    void loop();
    void execute();
    void updateState();

};

#endif // PROCESSOR_H
