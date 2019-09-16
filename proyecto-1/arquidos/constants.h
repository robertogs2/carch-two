#ifndef CONSTANTS_H
#define CONSTANTS_H

#define CACHE_BLOCKS_SIZE 8
#define MEMORY_BLOCKS_SIZE 16
#define CLOCK_HALF_PERIOD_MS 100
#define MEMORY_TICKS_READ 3
#define MEMORY_TICKS_WRITE 4
#define PROCESSORS_AMOUNT 4

/*!
    \class Constants
    \brief The Constants class defines the constants of the system
*/

class Constants{
public:
    inline static int clock_half_period_ms = CLOCK_HALF_PERIOD_MS;
};

#endif // CONSTANTS_H
