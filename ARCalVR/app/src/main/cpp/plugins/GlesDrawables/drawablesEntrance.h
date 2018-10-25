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
private:

protected:
    cvr::SubMenu *_mainMenu;
    cvr::MenuButton *_pointButton, *_planeButton, *_strokeButton;
    osg::ref_ptr<osg::Group> _root;
    cvr::SceneObject *rootSO;
    osg::ref_ptr<pointDrawable> _pointcloudDrawable;
    int _plane_num = 0;
    std::vector<planeDrawable*> _planeDrawables;
    osg::ref_ptr<strokeDrawable> _strokeDrawable;

    void initMenuButtons();
    void createObject(osg::Group *parent, osg::Vec3f pos);
    void createConvexPolygon(osg::Group *parent, osg::Vec3f pos);
public:
    bool init();
    void menuCallback(cvr::MenuItem * item);
    void preFrame();
    void postFrame();
};

#endif
