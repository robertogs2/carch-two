#ifndef CACHECONTROLLER_H
#define CACHECONTROLLER_H

#include "cache.h"
#include "processor.h"
#include "constants.h"
#include "buscontroller.h"
#include "coherencybus.h"

class Processor;
class BusController;

/*!
    \class CacheController
    \brief The CacheController class implements a simple cache controller

    CacheController implements a cache controller for a simple cache.
    It has the basic functions of accessing a cache block and check all
    its attributes, the data, the state, the tag, if its a hit or not, etc.
    Also, it simulates a connection between it and the bus controller to access
    the memory in the form of petitions. Before the memory petitions, the MSI protocol
    is implemented without the snooping segments.
*/

class CacheController{
public:

    // GUI/Stats attributes
    std::string nameTag_;
    int id_;
    size_t accesses_;
    size_t wmisses_;
    size_t rmisses_;
    std::string last_check_;
    std::string current_state_;

    //In case of memory, talk to the bus
    bool* waiting_memory_;
    bool* data_ready_;

    //Modules
    BusController* bus_controller_;
    Cache* cache_;
    Processor* processor_;
    CoherencyBus* coherency_bus_;

    CacheController(Cache* cache, Processor* processor, bool* waiting_memory);

    void setBusController(BusController* bus_controller);

    void updateCache(size_t memory_address, std::string data, BlockState block_state);

    void updateState(size_t memory_address, BlockState block_state);

    HitTuple accessCache(size_t memory_address, bool from_processor=true);

    void memory(size_t memory_address, bool read);

    void memoryPetition(size_t memory_address, bool read, std::string data, BlockState block_state);

    void setNameTag(size_t i);

    std::string stringController();

};

#endif // CACHECONTROLLER_H
