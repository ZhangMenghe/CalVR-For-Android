//
// Created by menghe on 8/20/2018.
//

#include "perfMonitor.h"

perfMonitor::perfMonitor()
        : current_FPS_(0),
          tv_last_sec_(0),
          last_tick_(0.f),
          tickindex_(0),
          ticksum_(0) {
    for (int32_t i = 0; i < kNumSamples; ++i) ticklist_[i] = 0;
}

double perfMonitor::UpdateTick(double currentTick) {
    ticksum_ -= ticklist_[tickindex_];
    ticksum_ += currentTick;
    ticklist_[tickindex_] = currentTick;
    tickindex_ = (tickindex_ + 1) % kNumSamples;

    return ((double)ticksum_ / kNumSamples);
}

float perfMonitor::Update() {
    struct timeval Time;
    gettimeofday(&Time, NULL);

    double time = Time.tv_sec + Time.tv_usec * 1.0 / 1000000.0;
    double tick = time - last_tick_;
    double d = UpdateTick(tick);
    last_tick_ = time;

    if (Time.tv_sec - tv_last_sec_ >= 1) {
        current_FPS_ = 1.f / d;
        tv_last_sec_ = Time.tv_sec;
    }
    return current_FPS_;
}