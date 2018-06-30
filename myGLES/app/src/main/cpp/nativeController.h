//
// Created by menghe on 6/29/2018.
//

#ifndef MYGLES_NATIVECONTROLLER_H
#define MYGLES_NATIVECONTROLLER_H

#include "utils.h"

namespace controller {
    class nativeController {
    public:
        nativeController();

        nativeController(AAssetManager *assetManager);

        ~nativeController();

        void onPause();

        void onResume();

        void onDrawFrame();

        void onTouched(float x, float y);

    private:
        AAssetManager *const _asset_manager;

    };
}



#endif //MYGLES_NATIVECONTROLLER_H
