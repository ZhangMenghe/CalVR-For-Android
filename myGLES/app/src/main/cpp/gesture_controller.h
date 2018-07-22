//
// Created by menghe on 7/22/2018.
//

#ifndef MYGLES_GESTURE_CONTROLLER_H
#define MYGLES_GESTURE_CONTROLLER_H


#include "utils.h"

class gesture_controller {
public:
    void createSphere(){
        LOGE("================called from native createShpere===============");
    }
    void onTouched(float x, float y){
        LOGE("================called from native onTouch================%f,%f",x, y );
    }

};


#endif //MYGLES_GESTURE_CONTROLLER_H
