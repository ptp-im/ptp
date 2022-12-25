/*!
 * toolbox.
 * time_profiler.cpp
 *
 * \date 11/30/2019
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#include "time_profiler.h"

#include "log.h"

#include <cstdint>

ptp_toolbox::time_profiler::~time_profiler() {
    timings.clear();
}
void ptp_toolbox::time_profiler::clear() {
    std::lock_guard<mutex_t> locker(lock);
    timings.clear();
}

size_t ptp_toolbox::time_profiler::size() const {
    std::lock_guard<mutex_t> locker(lock);
    std::size_t out = timings.size();

    return out;
}
void ptp_toolbox::time_profiler::begin(std::string tag) {
    std::lock_guard<mutex_t> locker(lock);
    if (timings.count(tag)) {
        return;
    }

    timings[tag] = hires_clock::now();
}
void ptp_toolbox::time_profiler::end(std::string tag, double* result) {
    std::lock_guard<mutex_t> locker(lock);
    if (!timings.count(tag)) {
        return;
    }

    hires_time_t past = timings[tag];
    hires_time_t now = hires_clock::now();
    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(now - past);
    timings.erase(tag);
    if (result != nullptr) {
        *result = time_span.count();
    } else {
        L_DEBUG_F(tag, "Profiling time: %lf ms", time_span.count());
    }
}
