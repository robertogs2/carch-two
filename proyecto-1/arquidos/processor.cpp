#include "processor.h"
Processor::Processor(std::string tag, int id, std::condition_variable* cv, bool* clock){
    tag_=tag;
    id_=id;

    instruction_count_=0;
    waiting_memory_=new bool(false);

    // Create a new cache
    cache_ = new Cache(id);

    // Create a cache controller and link it
    cache_controller_ = new CacheController(cache_, this, waiting_memory_);
    cache_controller_->setNameTag(id);
    // Generates new probabilistic distribution
    instruction_bernoulli_ = new Bernoulli(1, 0.0, 0.0);
    memory_binomial_ = new Binomial(0.5, MEMORY_BLOCKS_SIZE-1);

    // Condition variable for clock
    cv_ = cv;
    cv_m_ = new std::mutex();
    clock_=clock;
}

void Processor::setInstructionProbability(float p1, float p2, float p3){
    instruction_bernoulli_->setProbability(p1, p2, p3);
    instruction_bernoulli_->recalculateDistributions();
}

void Processor::setMemoryProbability(float p){
    memory_binomial_->setProbability(p, MEMORY_BLOCKS_SIZE-1);
}

size_t Processor::generateInstructionType(){
    return instruction_bernoulli_->calculate();
}

size_t Processor::generateMemoryAddress(){
    return memory_binomial_->calculate();
}

void Processor::loop(){
    while(true){
        //Locks mutex and waits for posedge
        std::unique_lock<std::mutex> lk(*cv_m_);
        cv_->wait(lk, [this]{return *this->clock_;});

        //Do stuff
        execute();

        //Waits for negedge
        cv_->wait(lk, [this]{return *this->clock_== false;});
    }
}

void Processor::execute(){
    std::string current_work ="";
    if(*waiting_memory_==false){ //not waiting for memory
        size_t to_execute = generateInstructionType();
        size_t memory_address = memory_binomial_->calculate();
        switch(to_execute){
            case 1: //Processing
                current_work="Processing";
                cache_controller_->current_state_="Processor is processing\nRead misses: " + std::to_string(cache_controller_->rmisses_)+
                        "\nWrite misses: " + std::to_string(cache_controller_->wmisses_)+"\nAccesses:" + std::to_string(cache_controller_->accesses_);
                break;
            case 2: //Reading
                current_work="Reading address: " + std::to_string(memory_address);
                cache_controller_->memory(memory_address, true);
                break;
            case 3: //Writing
                current_work="Writing address: " + std::to_string(memory_address);
                cache_controller_->memory(memory_address, false);
                break;
            default: //This case should not happen
                current_work="Error";
                break;
        }
        current_state_ = current_work;
        GUI::processor_states[id_] = tag_ + "\n"+ current_state_;
    }
    else if(*cache_controller_->data_ready_){ //remove the waiting
        *waiting_memory_=false;
        *cache_controller_->data_ready_=false;
        GUI::processor_states[id_] = tag_ + "\n" + current_state_ + "\nFinished waiting\n";
        cache_controller_->current_state_="Processor is processing\nRead misses: " + std::to_string(cache_controller_->rmisses_)+
                "\nWrite misses: " + std::to_string(cache_controller_->wmisses_)+"\nAccesses:" + std::to_string(cache_controller_->accesses_);
    }
    else{
        GUI::processor_states[id_] = tag_ + "\n" + current_state_ + "\nStill waiting\n";
    }
    GUI::cache_states[id_] = cache_controller_->cache_->stringCache();
    GUI::control_states[id_] = cache_controller_->stringController();
    GUI::change = true;
}
