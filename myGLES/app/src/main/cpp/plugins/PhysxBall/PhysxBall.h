#ifndef _PHYSXBALL_H
#define _PHYSXBALL_H

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
#include <foundation/PxVec3.h>
#include <osgText/Text>


class PhysxBall : public cvr::CVRPlugin, public cvr::MenuCallback
{
private:
    osgText::Text * _makeText(std::string text, float size, osg::Vec3 pos,
                             osg::Vec4 color, osgText::Text::AlignmentType align =
    osgText::Text::LEFT_CENTER);
    osg::ref_ptr<osg::Geometry> _makeQuad(float width, float height, osg::Vec4f color, osg::Vec3 pos);
  protected:
    cvr::SubMenu *_mainMenu;
    cvr::MenuButton * _addButton;
    osg::ref_ptr<osg::Group> _root, _balls;
    cvr::SceneObject *rootSO;
//    osg::PositionAttitudeTransform *_ball;

    bool initPhysX();

    void createBall(osg::Group* parent,osg::Vec3f pos, float radius);
    void createBoard(osg::Group* parent, osg::Vec3f pos);
    void createText(osg::Group* parent, osg::Vec3f pos);

    void addSphere(osg::Group*parent, osg::Vec3 pos, float radius);
public:
    bool init();
    void menuCallback(cvr::MenuItem * item);

};

#endif
