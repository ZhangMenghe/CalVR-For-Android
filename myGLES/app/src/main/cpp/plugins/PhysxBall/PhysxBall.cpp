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

using namespace osg;
using namespace cvr;
using namespace physx;

static PxPhysics * mPhysics;
static PxDefaultErrorCallback gDefaultErrorCallback;
static PxDefaultAllocator gDefaultAllocatorCallback;
static PxSimulationFilterShader gDefaultFilterShader = PxDefaultSimulationFilterShader;
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

    PxScene * gScene = mPhysics->createScene(*_sceneDesc);
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
    _root = new Switch;

    //--------------init physx-------------------
    if(!initPhysX())
        return false;

    return true;
}

void PhysxBall::menuCallback(cvr::MenuItem *item) {

}