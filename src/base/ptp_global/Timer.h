//
// Created by jack on 2023/1/9.
//

#ifndef PTP_TIMER_H
#define PTP_TIMER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

class Timer {
    std::atomic<bool> active{true};

public:
    void setTimeout(std::function<void()> function, int delay);
    void setInterval(std::function<void()> function, int interval);
    void stop();
    bool isStopped();

};

void Timer::setTimeout(std::function<void()> function, int delay) {
    active = true;
    std::thread t([=]() {
        if(!active.load()) return;
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        if(!active.load()) return;
        function();
    });
    t.detach();
}

void Timer::setInterval(std::function<void()> function, int interval) {
    active = true;
    std::thread t([=]() {
        while(active.load()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
            if(!active.load()) return;
            function();
        }
    });
    t.detach();
}

void Timer::stop() {
    active = false;
}

bool Timer::isStopped() {
    return !active;
}

#endif //PTP_TIMER_H
