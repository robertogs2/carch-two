#include "coherencybus.h"

CoherencyBus::CoherencyBus(Processor** processor_pointers, BusController* bus_controller){
    assignCacheControllers(processor_pointers);
    bus_controller_=bus_controller;
    bus_controller_->coherency_bus_=this;
    access_mutex_=new std::mutex;
}

void CoherencyBus::assignCacheControllers(Processor** processor_pointers){
    for(size_t i=0; i<PROCESSORS_AMOUNT;++i){
        cache_controllers_[i] = processor_pointers[i]->cache_controller_;
        cache_controllers_[i]->coherency_bus_=this;
    }
}

bool CoherencyBus::readRequest(CacheController* cache_controller, size_t memory_address, BlockState block_state, std::string& stateString){
    if(block_state==BlockState::Invalid){ // Case read and is invalid
        //find a cache with the data
        CacheController* cc=nullptr;
        HitTuple tuple = findCache(cache_controller, memory_address, cc);
        if(cc==nullptr){                                //No other cache has the data
            stateString = "No other cache found";
            return false;
        }
        else if(tuple.state==BlockState::Modified){     //Only other cache has it, in modified
            cache_controller->memoryPetition(memory_address,false,tuple.data,BlockState::Modified);     //Petition to write cache b data to memory
            cc->updateState(memory_address, BlockState::Shared);                                        //Update cache b block to shared
            cache_controller->updateCache(memory_address,tuple.data,BlockState::Shared);                //Update cache a data and set it to shared
            stateString = "Block was modified in cache " + std::to_string(cc->id_) + "\nTriggering write with this controller, and changed to shared";
        }
        else if (tuple.state==BlockState::Shared){      //Many other have the data, in shared
            cache_controller->updateCache(memory_address, tuple.data, BlockState::Shared);              //Update cache a data and set it to shared
            stateString = "Block was shared in cache" + std::to_string(cc->id_) + "\nCopying data";
        }
    }
    return true;
}

bool CoherencyBus::writeRequest(CacheController* cache_controller, size_t memory_address, std::string data, BlockState block_state, std::string& stateString){
    if(block_state==BlockState::Invalid){
        //find a cache with the data
        CacheController* cc=nullptr;
        HitTuple tuple = findCache(cache_controller, memory_address, cc);
        if(cc==nullptr){                                    //No other cache has the data
            stateString = "No other cache found";
            return false;
        }
        else if(tuple.state==BlockState::Modified){         //Only other cache has it, in modified
            cache_controller->memoryPetition(memory_address, false, tuple.data, BlockState::Modified);  //Petition to write cache b data to memory
            cc->updateState(memory_address, BlockState::Invalid);                                       //Update cache b block to invalid
            cache_controller->updateCache(memory_address,data,BlockState::Modified);                    //Update cache a block data, and set it to modified
            stateString = "Block was modified in cache " + std::to_string(cc->id_) + "\nTriggering write with this controller";
        }
        else if(tuple.state==BlockState::Shared){           //Many other have the data, in shared
            std::string invalidatedControllers = invalidateOthers(cache_controller, memory_address);                                         //Invalidate others with the address, share to invalid
            cache_controller->updateCache(memory_address,data,BlockState::Modified);                    //Update cache a block data, and set it to modified
            stateString = "Block was shared state in caches " + invalidatedControllers + "\nInvalidated those blocks";
        }
    }
    return true;
}

std::string CoherencyBus::invalidateOthers(CacheController* request_cache, size_t memory_address){
    std::string result = "";
    HitTuple tuple;
    for(size_t i=0; i<PROCESSORS_AMOUNT;++i){
        CacheController* cc = cache_controllers_[i];
        if(request_cache->id_!=cc->id_){ // Not the cache asking
            tuple = cc->accessCache(memory_address, false);
            if(!(tuple.state==BlockState::Invalid) && tuple.hit){ //Found one, tricky
                cc->updateState(memory_address, BlockState::Invalid);
                result += std::to_string(cc->id_)+ " ";
            }
        }
    }
    return result;
}

HitTuple CoherencyBus::findCache(CacheController* request_cache, size_t memory_address, CacheController*& found_cache){
    //found_cache=nullptr;
    HitTuple tuple;
    for(size_t i=0; i<PROCESSORS_AMOUNT;++i){
        CacheController* cc = cache_controllers_[i];
        if(request_cache->id_!=cc->id_){ // Not the cache asking for data
            tuple = cc->accessCache(memory_address, false);
            if(!(tuple.state==BlockState::Invalid) && tuple.hit){ //Found one, tricky
                found_cache=cc;
                i = found_cache->id_; // Supress compiler warning
                break;
            }
        }
    }
    return tuple;
}
