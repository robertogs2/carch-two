#include "cachecontroller.h"
#include <signal.h>

CacheController::CacheController(Cache* cache, Processor* processor, bool* waiting_memory){
    waiting_memory_=waiting_memory;
    data_ready_=new bool(false);

    cache_=cache;
    processor_=processor;

    cache->setNameTag(processor_->id_);

    accesses_=0;
    wmisses_=0;
    rmisses_=0;
    std::string lastCheck_="";
    std::string currentState_="";

}

void CacheController::setBusController(BusController* bus_controller){
    bus_controller_=bus_controller;
}

void CacheController::updateCache(size_t memory_address, std::string data, BlockState block_state){
    size_t block_address = memory_address%CACHE_BLOCKS_SIZE;
    size_t block_tag = memory_address/CACHE_BLOCKS_SIZE;
    cache_->updateBlock(block_address, block_tag, data, block_state);
}

void CacheController::updateState(size_t memory_address, BlockState block_state){
    size_t block_address = memory_address%CACHE_BLOCKS_SIZE;
    cache_->updateState(block_address, block_state);
}

HitTuple CacheController::accessCache(size_t memory_address, bool from_processor){
    size_t block_address = memory_address%CACHE_BLOCKS_SIZE;
    size_t block_tag = memory_address/CACHE_BLOCKS_SIZE;
    HitTuple tuple = {false, 0, "", BlockState::Invalid};
    CacheBlock block = cache_->blocks[block_address];

    tuple.hit=block.tag_ == block_tag;
    tuple.data=block.data_;
    tuple.memory_address=CACHE_BLOCKS_SIZE*block.tag_+block_address;//remaps to the address in the cache
    tuple.state=block.state_;

    if(from_processor) accesses_++;
    return tuple;
}


// Here is where all the politics need to be checkd
void CacheController::memory(size_t memory_address, bool read){
    HitTuple cache_state = accessCache(memory_address, read);
    std::string current_state = "";
    std::string coherency_state = "";
    if(read){
        if(cache_state.state==BlockState::Invalid){         // Read, invalid
            bool done = coherency_bus_->readRequest(this, memory_address, BlockState::Invalid, coherency_state);                         // Check if other cache has data and fix it
            if(!done){                                                                                                  // No other cache has data
                memoryPetition(memory_address, read, "", BlockState::Shared);                                           // Petition to read from memory to cache a and set it shared
            }
            current_state = "Read miss";
            ++rmisses_;
        }
        else if(cache_state.state==BlockState::Modified){
            if(cache_state.hit){                            // Read, modified, hit
                current_state = "Read hit";
            }
            else{                                           // Read, modified, miss
                memoryPetition(cache_state.memory_address, false, cache_state.data, BlockState::Modified);              // Petition to write cache a data to memory
                bool done = coherency_bus_->readRequest(this, memory_address, BlockState::Invalid, coherency_state);                     // Check if other cache has data and fix it
                if(!done){                                                                                              // No other cache has data
                    memoryPetition(memory_address, read, "", BlockState::Shared);                                       // Petition to read from memory to cache a and set it shared
                }
                current_state = "Read miss";
                ++rmisses_;
            }
        }
        else if(cache_state.state==BlockState::Shared){
            if(cache_state.hit){                            // Read, shared, hit
                current_state = "Read hit";
            }
            else{                                           // Read, shared, miss
                bool done = coherency_bus_->readRequest(this, memory_address, BlockState::Invalid, coherency_state);                     // Check if other cache has data and fix it
                if(!done){                                                                                              // No other cache has data
                    memoryPetition(memory_address, read, "", BlockState::Shared);                                       // Petition to read from memory to cache a and set it shared
                }
                current_state = "Read miss";
                ++rmisses_;
            }
        }
        else{
            raise(SIGSEGV);
        }
    }
    else{ //write request
        if(cache_state.state==BlockState::Invalid){         // Write, invalid
            bool done = coherency_bus_->writeRequest(this,memory_address,processor_->tag_,BlockState::Invalid, coherency_state);         // Check if other cache has data and fix it
            if(!done){                                                                                                  // No other cache has data
                updateCache(memory_address,processor_->tag_,BlockState::Modified);                                      // Write to cache a and set it modified
            }
            current_state = "Write miss";
            ++wmisses_;
        }
        else if(cache_state.state==BlockState::Modified){
            if(cache_state.hit){                            // Write, modified, hit
                updateCache(memory_address, processor_->tag_, BlockState::Modified);                                    // Write to cache a and keep it modified
                current_state = "Write hit";
            }
            else{                                           // Write, modifed, miss
                memoryPetition(cache_state.memory_address, read, cache_state.data, BlockState::Modified);               // Petition to write cache a data to memory
                bool done = coherency_bus_->writeRequest(this, memory_address, processor_->tag_, BlockState::Invalid, coherency_state);  // Check if other cache has data and fix it
                if(!done){                                                                                              // No other cache has data
                    updateCache(memory_address, processor_->tag_, BlockState::Modified);                                // Write to cache a and set it modified
                }
                current_state = "Write miss";
                ++wmisses_;
            }
        }
        else if(cache_state.state==BlockState::Shared){
            if(cache_state.hit){                            // Write, shared, hit
                updateCache(memory_address, processor_->tag_, BlockState::Modified);                                    // Write to cache a and set it modified
                coherency_bus_->invalidateOthers(this, memory_address);                                                 // Invalidate all others shared caches
                current_state = "Write hit";
            }
            else{                                           // Write, shared, miss
                bool done = coherency_bus_->writeRequest(this, memory_address, processor_->tag_, BlockState::Invalid, coherency_state);  // Check if other cache has data and fix it
                if(!done){                                                                                              // No other cache has data
                    updateCache(memory_address, processor_->tag_, BlockState::Modified);                                // Write to cache a and set it modified
                }
                current_state = "Write miss";
                ++wmisses_;
            }
        }
        else{
            raise(SIGSEGV);
        }
    }
    current_state_=current_state +
            "\nAccesses:" + std::to_string(accesses_)+
            "\nRead misses: " + std::to_string(rmisses_)+
            "\nWrite misses: " + std::to_string(wmisses_)+
            "\n"+coherency_state;
}

void CacheController::memoryPetition(size_t memory_address, bool read, std::string data, BlockState block_state){
    *data_ready_=false; //data not ready if its reading
    *waiting_memory_=true;
    bus_controller_->newPetition(this, memory_address, read, data, block_state);
}

void CacheController::setNameTag(size_t i){
    id_=i;
    std::ostringstream out;
    out << "Cache controller ";
    out << i;
    nameTag_=out.str();
}

std::string CacheController::stringController(){
    std::string result = nameTag_ +"\n" + current_state_;
    return result;
}
