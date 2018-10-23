//
// Created by menghe on 8/1/2018.
//
#ifndef MYGLES_GLTESTDRAWABLE_H
#define MYGLES_GLTESTDRAWABLE_H


#include <osg/Drawable>
#include <osg/Geode>
#include <stack>
#include <android/asset_manager.h>
#include <cvrUtil/AndroidHelper.h>
class glTestDrawable: public osg::Drawable {
protected:
    std::stack<cvr::glState>* _stateStack;
    AAssetManager *_asset_manager;
    bool PushAllState() const;
    bool PopAllState() const;
    osg::ref_ptr<osg::Geode> glNode;
public:
    virtual void Initialization(AAssetManager *manager,
                                std::stack<cvr::glState>* stateStack){
        _stateStack = stateStack;
        _asset_manager = manager;
    }
    osg::ref_ptr<osg::Geode> createDrawableNode(AAssetManager *manager,
                                                std::stack<cvr::glState>* stateStack){
        Initialization(manager, stateStack);
        glNode = new osg::Geode;
        glNode->addDrawable(this);
        setUseDisplayList(false);
        return glNode.get();
    }
    osg::ref_ptr<osg::Geode> getGLNode(){return glNode;}
};


#endif
