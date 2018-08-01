//
// Created by menghe on 8/1/2018.
//

#ifndef MYGLES_GLDRAWABLE_H
#define MYGLES_GLDRAWABLE_H


#include <osg/Drawable>
#include <stack>
#include <android/asset_manager.h>
#include "utils.h"
class glDrawable: public osg::Drawable {
protected:
    std::stack<utils::glState>* _stateStack;
    bool PushAllState() const;
    bool PopAllState() const;
public:
    virtual void Initialization(AAssetManager *manager,std::stack<utils::glState>* stateStack){_stateStack = stateStack;}
};


#endif //MYGLES_GLDRAWABLE_H
