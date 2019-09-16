#ifndef BUSCONTROLLER_H
#define BUSCONTROLLER_H

#include <vector>
#include "bus.h"
#include "cachecontroller.h"
#include "constants.h"
#include "coherencybus.h"

class CacheController;
class Processor;
class CoherencyBus;

/*!
    \class BusController
    \brief The BusController class controls the petitions made to the bus

    BusController simulates a FIFO queue for the multiple petitions made
    to the memory across the memory bus, each petition follows a struct
    which comes with the cache controller, the address, the data, and
    a read/write flag. Each petition is fullfilled during a clock cycle
    set in the clock_ variable.
*/

class BusController{

typedef struct {
    bool read;
    size_t memory_address;
    std::string data;
    BlockState block_state;
    CacheController* cache_controller_;
} Petition;

public:
    Bus* bus;
    CacheController* cache_controllers_[PROCESSORS_AMOUNT];
    CoherencyBus* coherency_bus_;

    //Synchronization variables
    std::condition_variable* cv_;
    std::mutex* cv_m_;
    bool* clock_;
    std::mutex* mtx;

    std::vector<Petition*>* petitions_;

    BusController(Processor** processor_pointers, std::condition_variable* cv, bool* clock);

    void assignCacheControllers(Processor** processor_pointers);

    void newPetition(CacheController* cache_controller, size_t memory_address, bool read, std::string data, BlockState block_state);

    void executePetition();

    bool cacheControllerIsQueued(CacheController* cc_in);

    void printPetitions();

    std::string stringBusController();

    std::string stringPetitions();

    void loop();

};

#endif // BUSCONTROLLER_H
