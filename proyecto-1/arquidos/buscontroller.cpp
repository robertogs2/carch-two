#include "buscontroller.h"

BusController::BusController(Processor** processor_pointers, std::condition_variable* cv, bool* clock){
    bus = new Bus();
    assignCacheControllers(processor_pointers);

    // Condition variable for clock
    cv_ = cv;
    cv_m_ = new std::mutex();
    clock_=clock;
    mtx=new std::mutex();;
    petitions_=new std::vector<Petition*>();

}

void BusController::assignCacheControllers(Processor** processor_pointers){
    for(size_t i=0; i<PROCESSORS_AMOUNT;++i){
        cache_controllers_[i] = processor_pointers[i]->cache_controller_;
        cache_controllers_[i]->setBusController(this);
    }
}

void BusController::newPetition(CacheController* cache_controller, size_t memory_address, bool read, std::string data, BlockState block_state){
    Petition* new_petition = new Petition;
    new_petition->cache_controller_=cache_controller;
    new_petition->read=read;
    new_petition->data=data;
    new_petition->memory_address=memory_address;
    new_petition->block_state=block_state;
    petitions_->push_back(new_petition);
}

void BusController::executePetition(){
    if(petitions_->size()>0){
        bus->working_=true;
        Petition* petition = petitions_->front();
        CacheController* cache_controller = petition->cache_controller_;
        if(petition->read){             //Petition is reading
            bus->read(petition->memory_address);
            if(*bus->done_){            //If bus finished reading
                cache_controller->updateCache(petition->memory_address, *bus->dataBus, petition->block_state);
            }
        }
        else{                           //Petition is writing
            bus->write(petition->memory_address, petition->data);
        }

        if(*bus->done_){
            petitions_->erase(petitions_->begin());                                         //Remove the completed request
            *cache_controller->data_ready_ = !cacheControllerIsQueued(cache_controller);    //Stall processor if completed one request but others are left, continue otherwise
        }
        else{
            *cache_controller->data_ready_=false;                                           //Stall processor if data is not ready
        }
    }
    else{
        bus->working_=false;
    }
}

bool BusController::cacheControllerIsQueued(CacheController* cc_in){
    for(size_t i =0; i < petitions_->size();++i){
        CacheController* cc = petitions_->at(i)->cache_controller_;
        if(cc_in->id_==cc->id_) return true;
    }
    return false;
}

void BusController::printPetitions(){
    for(size_t i =0; i < petitions_->size();++i){
        Petition* p = petitions_->at(i);
        std::cout << "Petition from: " << p->cache_controller_->processor_->tag_ << " to " << (p->read ? "read " : "write ") <<  "address" <<  p->memory_address << std::endl;
    }
    std::cout << std::endl;
}

std::string BusController::stringBusController(){
    std::string result= "Bus Controller \n";
    result += stringPetitions();
    return result;
}

std::string BusController::stringPetitions(){
    std::string result ="";
    for(size_t i =0; i < petitions_->size();++i){
        Petition* p = petitions_->at(i);
        result += "Petition from: " + p->cache_controller_->processor_->tag_ + " to " + (p->read ? "read from" : "write " + p->data +" to") + " address " + std::to_string(p->memory_address) + "\n";
    }
    return result;
}

void BusController::loop(){
    while(true){
        std::unique_lock<std::mutex> lk(*cv_m_);
        cv_->wait(lk, [this]{return *this->clock_;});

        //Do stuff
        executePetition();

        GUI::memory_state=bus->memory_->stringMemory();
        GUI::bus_state=bus->stringBus();
        GUI::bus_controller_state=stringBusController();
        GUI::change=true;

        cv_->wait(lk, [this]{return *this->clock_== false;});
    }
}
