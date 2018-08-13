//
// Created by menghe on 8/1/2018.
//

#ifndef MYGLES_GLDRAWABLE_H
#define MYGLES_GLDRAWABLE_H


#include <osg/Drawable>
#include <osg/Geode>
#include <stack>
#include <android/asset_manager.h>
#include "utils.h"
class glDrawable: public osg::Drawable {
protected:
    std::stack<utils::glState>* _stateStack;
    AAssetManager *_asset_manager;
    bool PushAllState() const;
    bool PopAllState() const;
public:
    virtual void Initialization(AAssetManager *manager,std::stack<utils::glState>* stateStack){
            _stateStack = stateStack;
            _asset_manager = manager;
    }
    osg::ref_ptr<osg::Geode> createDrawableNode(AAssetManager *manager,std::stack<utils::glState>* stateStack){
        Initialization(manager, stateStack);
        osg::ref_ptr<osg::Geode> glNode = new osg::Geode;
        glNode->addDrawable(this);
        setUseDisplayList(false);
        return glNode.get();
    }
};


#endif //MYGLES_GLDRAWABLE_H
