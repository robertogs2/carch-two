#ifndef MEMORY_H
#define MEMORY_H

#include <string>
#include <iostream>
#include "constants.h"

/*!
    \class Memory
    \brief The Memory class simulates a memory for a multiprocessor architecture

    Memory simulates a memory with its functions to read, and write, each of
    them with the latency associated. The main memmory is made out of an array
    of standard strings.
*/

class Memory{
public:

    std::string entries_[MEMORY_BLOCKS_SIZE];
    size_t read_state_;
    size_t write_state_;
    bool* done_;
    std::string* out_;

    Memory(){
        read_state_=write_state_=0;
        done_=new bool(false);
        out_=new std::string("");
        emptyMemory();
    }

    void read(size_t memory_address){
        *done_=false;
        if(++read_state_>=MEMORY_TICKS_READ){
            *done_=true;
            read_state_=0;
        }
        *out_ = entries_[memory_address];
    }

    void write(size_t memory_address, std::string data){
        *done_=false;
        if(++write_state_>=MEMORY_TICKS_WRITE){
            *done_=true;
            write_state_=0;
            entries_[memory_address]=data;
        }
        *out_="";
    }

    void emptyMemory(){
        for(size_t i = 0; i<MEMORY_BLOCKS_SIZE;++i){
            entries_[i] = "0";
        }
    }

    void printMemory(){
        for(size_t i = 0; i<MEMORY_BLOCKS_SIZE;++i){
            std::cout << i << ": " << entries_[i] << std::endl;
        }
    }

    std::string stringMemory(){
        std::string result ="Memory\n";
        for(size_t i = 0; i<MEMORY_BLOCKS_SIZE;++i){
            result+= ((i < 10) ? " " : "") + std::to_string(i)+": "+ entries_[i]+"\n";
        }
        return result;
    }

};

#endif // MEMORY_H
