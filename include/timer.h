#pragma once

#include "common.h"
#include <chrono>
#include <iostream>

NAMESPACE_MXT

class Timer
{
public:
    inline Timer() : stopped_(false)
    {
        start();  
    }

    inline void start()
    {
        start_ = std::chrono::high_resolution_clock::now();
    }

    inline double stop()
    {
        stopped_ = true;

        std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - start_;
 
        return diff.count();
    }

    inline ~Timer()
    {
        if (stopped_) return;

        std::cout << std::fixed << stop() << " s\n";
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
    bool stopped_;
};

NAMESPACE_MXT_END
