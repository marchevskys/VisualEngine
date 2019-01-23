#ifndef TIMER_H
#define TIMER_H
#include "logger.h"

#include <chrono>
#include <iostream>

class Timer {
    std::chrono::high_resolution_clock::time_point m_t;

  public:
    Timer() {
        m_t = std::chrono::high_resolution_clock::now();
    }
    inline void reset(const char *text) {
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - m_t);
        std::cout << text << " " << time_span.count() * 1000.0 << " ms" << std::endl;
        m_t = t2;
    }
};
#endif // TIMER_H
