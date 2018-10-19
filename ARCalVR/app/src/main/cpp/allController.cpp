#include "allController.h"
allController::allController(AAssetManager *assetManager){}

allController::~allController(){}

void allController::onCreate(const char * calvr_path){}

void allController::onPause(){}

void allController::onResume(void * env, void* context, void* activity){}

void allController::onDrawFrame(){}

void allController::onViewChanged(int rot, int width, int height){}

void allController::onResourceLoaded(const char* path){}

void allController::onSingleTouchDown(int pointer_num, float x, float y){}

void allController::onSingleTouchUp(int pointer_num, float x, float y){}

void allController::onDoubleTouch(int pointer_num, float x, float y){}

void allController::onTouchMove(int pointer_num, float x, float y){}

float allController::getFPS(){return 60.0f;}