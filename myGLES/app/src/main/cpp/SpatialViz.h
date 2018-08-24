#ifndef _SPATIALVIZ_H
#define _SPATIALVIZ_H

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

// OSG
#include <osg/Group>
#include <osg/Vec3>
#include <osg/Uniform>
#include <osgDB/ReadFile>
#include <osgDB/FileUtils>

#include <osg/PositionAttitudeTransform> 	// the transform of objects 


class SpatialViz : public cvr::CVRPlugin, public cvr::MenuCallback
{
  protected:
    cvr::MenuCheckbox * testItem1;
    cvr::MenuCheckbox * testItem2;
    cvr::MenuCheckbox * testItem3;
 
  public:
    bool init();
    void menuCallback(cvr::MenuItem * item);

    
};

#endif
