//
// Created by menghe on 6/29/2018.
//

#include "nativeController.h"

using namespace controller;

nativeController::nativeController(AAssetManager * assetManager)
        : _asset_manager(assetManager){}

nativeController::~nativeController() {}

void nativeController::onTouched(float x, float y) {
    LOGE("================called from native onTouch================");
}