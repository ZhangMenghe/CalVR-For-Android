//
// Created by menghe on 8/20/2018.
//

#ifndef ARCALVR_PERFMONITOR_H
#define ARCALVR_PERFMONITOR_H


#include <cstdint>
#include <ctime>
const int32_t kNumSamples = 100;
class perfMonitor {
private:
    float current_FPS_;
    time_t tv_last_sec_;

    double last_tick_;
    int32_t tickindex_;
    double ticksum_;
    double ticklist_[kNumSamples];

    double UpdateTick(double current_tick);

public:
    perfMonitor();
    float Update();

    static double GetCurrentTime() {
        struct timeval time;
        gettimeofday(&time, NULL);
        double ret = time.tv_sec + time.tv_usec * 1.0 / 1000000.0;
        return ret;
    }
};


#endif //MYGLES_PERFMONITOR_H
