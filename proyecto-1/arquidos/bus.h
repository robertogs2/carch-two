#ifndef BUS_H
#define BUS_H

#include "memory.h"

/*!
    \class Bus
    \brief The Bus class simulates a memory bus for a multiprocessor architecture

    Bus simulates the use of a hardware bus connected to a memory to access it.
    It has the functions to read and write to simulate the process of sending
    the signals to memory to read and write.
*/

class Bus{

public:
    Memory* memory_;
    std::string* dataBus;
    bool* done_;

    // GUI
    size_t memory_address_;
    bool working_;
    bool read_;

    Bus(){
        memory_=new Memory();
        dataBus=new std::string();
        done_=memory_->done_;

        memory_address_=0;
    }

    void read(size_t memory_address){
        read_=true;
        memory_address_=memory_address;
        memory_->read(memory_address);
        if(*memory_->done_){
            *dataBus = *memory_->out_;
            *done_=true;
        }
    }

    void write(size_t memory_address, std::string data){
        read_=false;
        memory_address_=memory_address;
        memory_->write(memory_address, data);
        if(*memory_->done_){
            *done_=true;
        }
    }

    std::string stringBus(){
        std::string result ="Bus\n";
        if(working_){
            if(read_) {
                result += "Reading address " + std::to_string(memory_address_) + (*done_ ? " done\n" : "\nMissing cycles: "+std::to_string(MEMORY_TICKS_READ - memory_->read_state_)+"\n");
            }
            else{
                result += "Writing address " + std::to_string(memory_address_) + (*done_ ? " done\n" : "\nMissing cycles: "+std::to_string(MEMORY_TICKS_WRITE - memory_->write_state_)+"\n");
            }
        }
        else{
            result = "Bus\nDoing nothing\n";
        }
        return result;
    }

};

#endif // BUS_H
