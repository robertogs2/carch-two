#ifndef COHERENCYBUS_H
#define COHERENCYBUS_H

#include "cachecontroller.h"
#include "buscontroller.h"
#include "constants.h"
#include <mutex>

class CacheController;
class BusController;
class Processor;

/*!
    \class CoherencyBus
    \brief The CoherencyBus class implements a simple coherency bus for
    the MSI protocol

    CoherencyBus implements a set of functions to simulate coherency
    between a set of cache controllers. The MSI protocol of the snooping
    segment is implemented in here as a simulation of a directory-based
    protocol.
*/



class CoherencyBus{
public:

    CacheController* cache_controllers_[PROCESSORS_AMOUNT];
    BusController* bus_controller_;
    std::mutex* access_mutex_;

    CoherencyBus(Processor** processor_pointers, BusController* bus_controller);
    void assignCacheControllers(Processor** processor_pointers);

    bool readRequest(CacheController* cache_controller, size_t memory_address, BlockState block_state, std::string& stateString);
    bool writeRequest(CacheController* cache_controller, size_t memory_address, std::string data, BlockState block_state, std::string& stateString);
    HitTuple findCache(CacheController* request_cache, size_t memory_address, CacheController*& found_cache);
    std::string invalidateOthers(CacheController* request_cache, size_t memory_address);
};

#endif // COHERENCYBUS_H
