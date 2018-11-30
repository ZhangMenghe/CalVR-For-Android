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


// PhysX:
#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultErrorCallback.h>
#include <extensions/PxDefaultAllocator.h>
#include <foundation/Px.h>
#include <extensions/PxShapeExt.h>
#include <foundation/PxMat33.h>
#include <foundation/PxQuat.h>



class SpatialViz : public cvr::CVRPlugin, public cvr::MenuCallback
{
  protected:
    // menu and CVR variables
    cvr::SubMenu *_mainMenu;
    cvr::MenuButton *_mazePuzzleButton, *_5x5puzzleButton, *_tetrisPuzzle2, *_labyrinthPuzzle, *_tetrisPuzzle, *_removePuzzles, *_restartPhysics;
    cvr::MenuCheckbox * _rotate5x5Menu, *_rotateMazeMenu, *_rotateLabMenu, *_rotateMainTetrisMenu, *_rotateMainTetris2Menu;
    cvr::SceneObject *soLab, *so5x5, *soMaze, *soTetris, *soMainTetris, *soTetris2, *soMainTetris2;

    // testing Popup Menu Button
    cvr::MenuButton *_testButton;
    cvr::PopupMenu *popup;

    osg::PositionAttitudeTransform *_sphereTrans, *_cubeTrans;
    osg::Geode *_cubeGeode, *_sphereGeode;
    osg::Switch *_root, *_labyrinthSwitch, *_5x5Switch, *_mazeSwitch, *_tetrisSwitch, *_mainTetrisSwitch, *_tetrisSwitch2, *_mainTetrisSwitch2, *_testSwitch;
    
    // Puzzle variables
    // used to rotate the entire puzzle
    osg::PositionAttitudeTransform * _puzzleMazeTrans, *_puzzle5x5Trans, *_labTrans, *_mainTetrisPieceTrans, *_mainTetris2PieceTrans;
    // positions for the transforms
    osg::Vec3 _labPos;
    osg::PositionAttitudeTransform  *_mazeBox, * _piecePuzzle1, * _piecePuzzle2, * _piecePuzzle3, * _piecePuzzle4, * _piecePuzzle5;
    osg::Group * _puzzleMazeGroup, *_puzzle5x5Group, *_piecePuzzleGroup, *_labyrinthGroup;
    osg::Group * _objGroup5x5, *_objGroupMaze, *_objGroupTetris, *_TetrisPiece, *_mainTetris, *_TetrisPiece2, *_mainTetris2;
    
    physx::PxSceneDesc* _sceneDesc;

    // ----- ANDROID additions ----- //
    osg::Group *_androidGroup;          // contains all the AR-Core objects
    cvr::SceneObject *_androidSO;

    int _objNum = 0, _plane_num = 0;    // keeps track of the detected anchor points and planes

    // stores the points and planes
    osg::ref_ptr<pointDrawable> _pointcloudDrawable;
    std::vector<planeDrawable*> _planeDrawables;

    bool inWorldSpace = true;
    void addCubeWorld(osg::Group*, osg::Matrixf);
    osg::Vec2f _mPreviousPos;
    // ----- end ANDROID additions ----- //
    
    // osg helper functions
    void setNodeTransparency(osg::Node*, float);
    osg::PositionAttitudeTransform  * addSphere(osg::Group*, osg::Vec3, float, osg::Vec3);
    osg::PositionAttitudeTransform  * addCube(osg::Group*, osg::Vec3, float, float, float, osg::Vec3);
    
    // PhysX helper functions
    void updateGravity(osg::Quat, physx::PxScene *);
    osg::Vec2 checkTetris_matching(osg::Quat);
    osg::Vec2 checkTetris2_matching(osg::Quat);
 
  public:
    SpatialViz();
    virtual ~SpatialViz();
    
    bool init();
    void menuCallback(cvr::MenuItem * item);
    void resetSceneManager();
    void preFrame();
    
    // PhysX 
    bool initPhysX();
    void restartPhysics();
    
    // puzzles
    void createTetris(int);
    void createTetris2(int);
    void createPuzzleCube(int);
    void create5x5(int);
    void createLabyrinth(float, float);

    /* createBoxes and createSpheres take in dimensions in terms of m */
#if(__ANDROID__)
    // createIdentity() and createZero() are deprecated since 3.3
    void createBoxes(physx::PxVec3, physx::PxVec3, osg::Group*, std::vector<osg::PositionAttitudeTransform*>*, std::vector<physx::PxRigidDynamic*>*, std::vector<osg::Vec3>*, std::vector<physx::PxVec3>*, physx::PxQuat quat = physx::PxQuat(physx::PxIDENTITY()));
#else
    void createBoxes(physx::PxVec3, physx::PxVec3, osg::Group*, std::vector<osg::PositionAttitudeTransform*>*, std::vector<physx::PxRigidDynamic*>*, std::vector<osg::Vec3>*, std::vector<physx::PxVec3>*, physx::PxQuat quat = physx::PxQuat::createIdentity());
#endif
    void createSpheres(physx::PxVec3, float, osg::Group*, std::vector<osg::PositionAttitudeTransform*>*, std::vector<physx::PxRigidDynamic*>*, std::vector<osg::Vec3>*, std::vector<physx::PxVec3>*);

    // ----- more ANDROID additions ----- //
    bool processEvent(cvr::InteractionEvent *event);
    // ----- end more ANDROID additions ----- //

};

#endif
