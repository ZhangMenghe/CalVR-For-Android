#include "PhysxBall.h"
//PhysX
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/Px.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxMat33.h>
#include <foundation/PxQuat.h>
#include <extensions/PxRigidActorExt.h>
#include <foundation/PxFoundation.h>
#include <osg/ShapeDrawable>
#include <cvrMenu/BoardMenu/BoardMenuGeometry.h>

using namespace osg;
using namespace cvr;
using namespace physx;

static PxPhysics * mPhysics;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
PxScene * gScene;
bool PhysxBall::initPhysX(){
    PxFoundation *mFoundation = NULL;
    mFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);

    PxTolerancesScale mToleranceScale;
    mToleranceScale.length = 50;//units in cm, 50cm
    mToleranceScale.speed = 981;
    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale);

#if(PX_PHYSICS_VERSION >= 34)
    // PX_C_EXPORT bool PX_CALL_CONV 	PxInitExtensions (physx::PxPhysics &physics, physx::PxPvd *pvd) since 3.4
	if (!PxInitExtensions(*mPhysics, nullptr)) {
        return false;
    }
#else
    if (!PxInitExtensions(*mPhysics)){
        return false;
    }
}
#endif
    if(!mPhysics) return false;

    //------------------create physics scene-----------------
    PxSceneDesc * _sceneDesc = new PxSceneDesc(mToleranceScale);
    _sceneDesc->gravity = PxVec3(.0f, -9.81f, .0f);

    if(!_sceneDesc->cpuDispatcher)
    {
        PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
        if(!mCpuDispatcher) return false;
        _sceneDesc->cpuDispatcher = mCpuDispatcher;
    }
    if(!_sceneDesc->filterShader)
        _sceneDesc->filterShader  = gDefaultFilterShader;

    gScene = mPhysics->createScene(*_sceneDesc);
    if(!gScene) return false;
    gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0);
    gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

    //---------------------create plane-------------------
    PxMaterial* mMaterial = mPhysics->createMaterial(0.1,0.2,0.5);
    PxTransform pose = PxTransform(PxVec3(0.0f, -0.25, 0.0f),PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
    PxRigidStatic* plane = mPhysics->createRigidStatic(pose);
    PxShape *shape = PxRigidActorExt::createExclusiveShape(*plane, PxPlaneGeometry(), *mMaterial);
    if(!shape) return false;
    gScene->addActor(*plane);

    return true;
}
bool PhysxBall::init() {
    // --------------- create the menu ---------------
    _mainMenu = new SubMenu("PhysxBall", "PhysxBall");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);

    //---------------create the button------------
    _addButton = new MenuButton("Add Ball");
    _addButton->setCallback(this);
    _mainMenu->addItem(_addButton);

    //--------------init scene node--------------
    _root = new Group;
    _balls = new Group;
    _root->addChild(_balls);
    //--------------init physx-------------------
    if(!initPhysX())
        return false;

    SceneManager::instance()->getSceneRoot()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    Vec3f boardPos = Vec3f(0, 10.0f, 0);
    createBall(_balls, osg::Vec3(.0f,0.5f,.0f), 0.05f);
    createBoard(_root, boardPos);
    createText(_root, boardPos + Vec3f(0.1f,-2,0.1f));

    //bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds
    rootSO= new SceneObject("myPluginRoot", false, false, false, false, false);

    rootSO->addChild(_root);
    PluginHelper::registerSceneObject(rootSO, "PhysxBallSceneObjset");
    rootSO->dirtyBounds();
    rootSO->attachToScene();

    return true;
}

void PhysxBall::menuCallback(cvr::MenuItem *item) {
    if(item == _addButton)
        createBall(_balls, Vec3(.0f,0.5f,.0f), 0.05f);
}
ref_ptr<Geometry> PhysxBall::_makeQuad(float width, float height, osg::Vec4f color, osg::Vec3 pos) {
    ref_ptr<Geometry> geo = new osg::Geometry();
    geo->setUseDisplayList(false);
    geo->setUseVertexBufferObjects(true);

    osg::Vec3Array* verts = new osg::Vec3Array();
    verts->push_back(pos);
    verts->push_back(pos + osg::Vec3(0, 0, height));
    verts->push_back(pos + osg::Vec3(width, 0, height));
    verts->push_back(pos + osg::Vec3(width,0,0));

    geo->setVertexArray(verts);

    geo->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS,0,4));

    osg::Vec4Array* colors = new osg::Vec4Array;
    colors->push_back(color);
    geo->setColorArray(colors);
    geo->setColorBinding(osg::Geometry::BIND_OVERALL);

    osg::Vec2Array* texcoords = new osg::Vec2Array;
    texcoords->push_back(osg::Vec2(0,0));
    texcoords->push_back(osg::Vec2(0, 1));
    texcoords->push_back(osg::Vec2(1, 1));
    texcoords->push_back(osg::Vec2(1,0));

    geo->setTexCoordArray(0,texcoords);

    return geo.get();
}
osgText::Text * PhysxBall::_makeText(std::string text, float size,
                                            osg::Vec3 pos, osg::Vec4 color, osgText::Text::AlignmentType align)
{
    osgText::Text * textNode = new osgText::Text();
    textNode->setCharacterSize(size);
    textNode->setAlignment(align);
    textNode->setPosition(pos);
    textNode->setColor(color);
    textNode->setBackdropColor(osg::Vec4(0,0,0,0));
    textNode->setAxisAlignment(osgText::Text::XZ_PLANE);
    textNode->setText(text);
    return textNode;
}

void PhysxBall::createText(Group* parent, osg::Vec3f pos) {
    osg::Geode * geode = new osg::Geode();
    for(int i=0; i<5; i++)
        geode->addDrawable(_makeText("Line " + std::to_string(i+1),
                                     0.1,
                                     pos+Vec3f(.0f, .0f, 0.2f * i),
                                     Vec4f(1.0f,0.6f,0.2f,1.0f)));
//    osgText::Text * textNode = _makeText("Line 1", 0.1, pos, _textColor);
//    geode->addDrawable(textNode);
    parent->addChild(geode);
}
void PhysxBall::createBoard(Group* parent, osg::Vec3f pos) {
    osg::Geode * geode = new osg::Geode();

    geode->addDrawable(
            _makeQuad(2, 2, Vec4f(.0f,.0f,.0f,1.0f), pos));
//    geode->addDrawable(
//            _makeQuad(1, 1, Vec4f(1.0f, 1.0f, 1.0f, 1.0f), pos + Vec3f(0,-2,0)));
    parent->addChild(geode);
}

void PhysxBall::createBall(osg::Group* parent,osg::Vec3f pos, float radius) {
//    PxReal density = 1.0f;
//    PxMaterial* mMaterial = mPhysics->createMaterial(0.1,0.2,0.5);
//    PxSphereGeometry geometrySphere(radius);
//    PxTransform transform(pxPos, PxQuat(PxIDENTITY()));
//    PxRigidDynamic *actor = PxCreateDynamic(*mPhysics, transform, geometrySphere, *mMaterial, density);
//    actor->setAngularDamping(0.75);
//    actor->setLinearVelocity(PxVec3(0,0,0));
//    actor->setSleepThreshold(0.0);
//    if(!actor) return;
//    gScene->addActor(*actor);
//    //TODO: should change to register, solve bounding problems
    addSphere(parent, pos, radius);

}
void PhysxBall::addSphere(osg::Group*parent, osg::Vec3 pos, float radius)
{
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable();
    shape->setShape(new osg::Sphere(pos, radius));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    shape->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    node->addDrawable(shape.get());
    parent->addChild(node.get());
}