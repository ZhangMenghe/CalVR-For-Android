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
#include <PxRigidActor.h>

#include "Engine.h"
/** The callback to update the actor, should be applied to a matrix transform node */
class UpdateActorCallback : public osg::NodeCallback
{
public:
    UpdateActorCallback( physx::PxRigidActor* a=0 ) : _actor(a) {}

    UpdateActorCallback( const UpdateActorCallback& copy, const osg::CopyOp& op=osg::CopyOp::SHALLOW_COPY )
            : osg::NodeCallback(copy, op), _actor(copy._actor) {}

//    META_Object( osgPhysics, UpdateActorCallback );

    virtual void operator()( osg::Node* node, osg::NodeVisitor* nv );

protected:
    physx::PxRigidActor* _actor;
};

class PhysxBall : public cvr::CVRPlugin, public cvr::MenuCallback
{
private:
    osgText::Text * _makeText(std::string text, float size, osg::Vec3 pos,
                             osg::Vec4 color, osgText::Text::AlignmentType align =
    osgText::Text::LEFT_CENTER);
    osg::ref_ptr<osg::Geometry> _makeQuad(float width, float height, osg::Vec4f color, osg::Vec3 pos);

    void createBall(osg::Group* parent, osg::Vec3f pos, float radius);
    void createText(osg::Group* parent, osg::Vec3f pos);
    void createPlane(osg::Group* parent, osg::Vec3f pos);

    void addBoard(osg::Group* parent, osg::Vec3f pos, osg::Vec3f rotAxis = osg::Vec3f(.0f,.0f,.0f),float rotAngle = .0f );
    osg::ref_ptr<osg::MatrixTransform> addSphere(osg::Group*parent, osg::Vec3 pos, float radius);
protected:
    osgPhysx::Engine * _phyEngine;
    cvr::SubMenu *_mainMenu;
    cvr::MenuButton * _addButton;
    osg::ref_ptr<osg::Group> _menu, _scene;
    cvr::SceneObject *rootSO, *sceneSO, *menuSo;
//    osg::ref_ptr<osg::MatrixTransform> sphereTrans;
//    osg::PositionAttitudeTransform *_ball;


public:
    bool init();
    void menuCallback(cvr::MenuItem * item);
    void preFrame();
//    void operator()( osg::Node* node, osg::NodeVisitor* nv );
};

#endif
