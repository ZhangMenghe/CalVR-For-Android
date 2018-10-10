#include "Engine.h"
#include <osg/io_utils>
//#include "PhysicsUtil.h"
#include <algorithm>
#include <iostream>

using namespace osgPhysx;
using namespace physx;

PxReal Engine::_defaultTimestep = 1.0f/60.0f;

Engine* Engine::instance()
{
    static osg::ref_ptr<Engine> s_registry = new Engine;
    return s_registry.get();
}

Engine::Engine()
:   _cooking(NULL)
{
    PxFoundation* foundation = PxCreateFoundation( PX_PHYSICS_VERSION, _defaultAllocatorCallback, _defaultErrorCallback );
    if ( !foundation )
    {
        OSG_WARN << "Unable to initialize PhysX foundation." << std::endl;
        return;
    }
    
    _physicsSDK = PxCreatePhysics( PX_PHYSICS_VERSION, *foundation, PxTolerancesScale() );
    if ( !_physicsSDK ) 
    {
        OSG_WARN << "Unable to initialize PhysX SDK." << std::endl;
        return;
    }
    _defaultMaterial = _physicsSDK->createMaterial( 0.5, 0.5, 0.5 );

#if(PX_PHYSICS_VERSION >= 34)
    // PX_C_EXPORT bool PX_CALL_CONV 	PxInitExtensions (physx::PxPhysics &physics, physx::PxPvd *pvd) since 3.4
    if (!PxInitExtensions(*_physicsSDK, nullptr)) {
        return;
    }
#else
    if (!PxInitExtensions(*mPhysics)){
        return;
    }
}
#endif
}

Engine::~Engine()
{
    clear();
    PxCloseExtensions();
    _defaultMaterial->release();
    _physicsSDK->release();
    if ( _cooking ) _cooking->release();
}

bool Engine::addScene( const std::string& name, PxScene* s )
{
    if ( !s || _sceneMap.find(name)!=_sceneMap.end() ) return false;
    _sceneMap[name] = s;
    return true;
}

bool Engine::removeScene( const std::string& name, bool doRelease )
{
    SceneMap::iterator itr = _sceneMap.find( name );
    if ( itr==_sceneMap.end() ) return false;
    
    if ( doRelease )
    {
        releaseActors( itr->second );
        itr->second->release();
    }
    _sceneMap.erase( itr );
    return true;
}

PxScene* Engine::getScene( const std::string& name )
{
    SceneMap::iterator itr = _sceneMap.find( name );
    if ( itr==_sceneMap.end() ) return NULL;
    return itr->second;
}

const PxScene* Engine::getScene( const std::string& name ) const
{
    SceneMap::const_iterator itr = _sceneMap.find( name );
    if ( itr==_sceneMap.end() ) return NULL;
    return itr->second;
}

bool Engine::addActor( const std::string& s, PxActor* actor )
{
    PxScene* scene = getScene(s);
    if ( !scene || !actor ) return false;
    scene->addActor( *actor );
    _actorMap[scene].push_back( actor );
    return true;
}

//bool Engine::addActor( const std::string& s, PxRigidActor* actor, const PxFilterData& filter )
//{
//    if ( addActor(s, actor) )
//        return createSimulationFilter(actor, filter);
//    else
//        return false;
//}
//
//bool Engine::addActor( const std::string& s, physx::PxParticleBase* ps, const physx::PxFilterData& filter )
//{
//    if ( addActor(s, ps) )
//    {
//        ps->setSimulationFilterData( filter );
//        return true;
//    }
//    else
//        return false;
//}

bool Engine::removeActor( const std::string& s, PxActor* actor )
{
    PxScene* scene = getScene(s);
    if ( !scene || !actor ) return false;
    
    ActorMap::iterator itr = _actorMap.find( scene );
    if ( itr==_actorMap.end() ) return false;
    
    ActorList& actors = itr->second;
    ActorList::iterator fitr = std::find( actors.begin(), actors.end(), actor );
    if ( fitr==actors.end() ) return false;
    
    scene->removeActor( *actor );
    actors.erase( fitr );
    if ( !actors.size() ) _actorMap.erase( itr );
    return true;
}

PxCooking* Engine::getOrCreateCooking( PxCookingParams* params, bool forceCreating )
{
    if ( forceCreating && _cooking )
    {
        _cooking->release();
        _cooking = NULL;
    }
    
    if ( !_cooking )
    {
        if ( params )
            _cooking = PxCreateCooking( PX_PHYSICS_VERSION, _physicsSDK->getFoundation(), *params );
        else
        {
            physx::PxTolerancesScale sc;
            physx::PxCookingParams defParams( sc );
            _cooking = PxCreateCooking( PX_PHYSICS_VERSION, _physicsSDK->getFoundation(), defParams );
        }
    }
    return _cooking;
}

void Engine::update( double step )
{
    for ( SceneMap::iterator itr=_sceneMap.begin(); itr!=_sceneMap.end(); ++itr )
    {
        PxScene* scene = itr->second;
        scene->simulate( step );
        while( !scene->fetchResults() ) { /* do nothing but wait */ }
    }
}

void Engine::clear()
{
    for ( SceneMap::iterator itr=_sceneMap.begin(); itr!=_sceneMap.end(); ++itr )
    {
        PxScene* scene = itr->second;
        releaseActors( scene );
        scene->release();
    }
    _sceneMap.clear();
    _actorMap.clear();
}

void Engine::releaseActors( PxScene* scene )
{
    ActorMap::iterator itr = _actorMap.find( scene );
    if ( itr==_actorMap.end() ) return;
    
    ActorList& actors = itr->second;
    for ( unsigned int i=0; i<actors.size(); ++i )
    {
        scene->removeActor( *(actors[i]) );
    }
    _actorMap.erase( itr );
}
