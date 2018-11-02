#ifndef DRAWABLE_ENTRANCE_H
#define DRAWABLE_ENTRANCE_H

// STD
#include <queue>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

// CVR
#include <cvrKernel/CVRPlugin.h>
#include <cvrKernel/ScreenBase.h>
#include <cvrKernel/SceneManager.h>
#include <cvrKernel/SceneObject.h>
#include <cvrKernel/Navigation.h>
#include <cvrKernel/PluginHelper.h>
#include <cvrKernel/ComController.h>
#include <cvrMenu/MenuSystem.h>
#include <cvrMenu/SubMenu.h>
#include <cvrMenu/MenuButton.h>
#include <cvrMenu/MenuCheckbox.h>
#include <cvrMenu/MenuRangeValue.h>
#include <cvrConfig/ConfigManager.h>
#include <cvrKernel/FileHandler.h>
#include <cvrUtil/AndroidHelper.h>
// OSG
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Uniform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include "pointDrawable.h"
#include "planeDrawable.h"
#include "strokeDrawable.h"

class GlesDrawables : public cvr::CVRPlugin, public cvr::MenuCallback
{
typedef osg::ref_ptr<osg::MatrixTransform> Transform;
protected:
    cvr::SubMenu *_mainMenu;
    cvr::MenuButton *_pointButton, *_planeButton, *_strokeButton;
    osg::Group *_root, *_objects;
    cvr::SceneObject *rootSO, *objSO;
    osg::ref_ptr<pointDrawable> _pointcloudDrawable;
    int _plane_num = 0, _objNum = 0;
    std::vector<planeDrawable*> _planeDrawables;
    osg::ref_ptr<strokeDrawable> _strokeDrawable;
    std::unordered_map<osg::Node*, osg::Uniform*> _map;

    int _hitTimes = 0;
    const osg::Vec4f _switchColor[2] = {osg::Vec4f(.0f,1.0f,.0f,1.0f),
                                        osg::Vec4f(1.0f,.0f,.0f,1.0f)};

    void initMenuButtons();
    void createObject(osg::Group *, const char*, const char*, osg::Matrixf);
    void createObject(osg::Group *, const char*, const char*, osg::Matrixf, bool opengl);
    void tackleHitted(osgUtil::LineSegmentIntersector::Intersection result );
public:
    bool init();
    void menuCallback(cvr::MenuItem * item);
    void preFrame();
    void postFrame();
};

#endif
