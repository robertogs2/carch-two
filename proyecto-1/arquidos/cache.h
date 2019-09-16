#ifndef CACHE_H
#define CACHE_H

#include<string>
#include<iostream>
#include <sstream>
#include "constants.h"

enum class BlockState{
    Invalid, Modified, Shared
};

typedef struct{
    BlockState state_;
    size_t tag_;
    std::string data_;
} CacheBlock;

typedef struct{
    bool hit;
    size_t memory_address;
    std::string data;
    BlockState state;
} HitTuple;

/*!
    \class Cache
    \brief The Cache class implements a simple cache memory

    Cache implements a simple cache memory of 8 blocks, each
    of them with an associated block state for the MSI protocol,
    a tag, and a data. This is simulated as an array of blocks.
*/

class Cache{
public:
    // Name attributes
    std::string nameTag_;
    int id_;

    bool writing_;
    CacheBlock blocks[CACHE_BLOCKS_SIZE];

    Cache(int id){
        writing_=false;
        resetCache();
        setNameTag(id);
    }

    void resetCache(){
        for(size_t i=0; i<CACHE_BLOCKS_SIZE;++i){
            blocks[i]={BlockState::Invalid, 0, "0"};
        }
    }

    void printCache(){
        std::cout << stringCache();
    }

    std::string stringCache(){
        std::string result =nameTag_ +"\n";
        for(size_t i=0; i<CACHE_BLOCKS_SIZE;++i){
            CacheBlock block = blocks[i];
            result +=  std::string("V: ") + stateString(block.state_) + std::string("| t: ") + std::to_string(block.tag_) + std::string("| d: ") + block.data_ + std::string("\n");
        }
        result += "\n";
        return result;
    }

    void invalidateBlock(size_t index){
        blocks[index].state_=BlockState::Invalid;
    }

    void updateState(size_t index, BlockState block_state){
        blocks[index].state_=block_state;
    }

    void updateBlock(size_t index, size_t tag, std::string data, BlockState state=BlockState::Modified){
        blocks[index].data_=data;
        blocks[index].tag_=tag;
        blocks[index].state_=state;
    }

    void setNameTag(size_t i){
        id_=i;
        std::ostringstream out;
        out << "Cache ";
        out << i;
        nameTag_=out.str();
    }

    std::string stateString(BlockState state){
        if(state == BlockState::Invalid) return "I";
        if(state == BlockState::Modified) return "M";
        if(state == BlockState::Shared) return "S";
        return "";
    }
};

#endif // CACHE_H
