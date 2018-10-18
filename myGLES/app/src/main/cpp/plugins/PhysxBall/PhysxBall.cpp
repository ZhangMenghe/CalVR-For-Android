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
#include "PhysicsUtils.h"
#include <cvrUtil/ARCoreHelper.h>
#include <gtc/type_ptr.hpp>
#include <gtc/matrix_transform.hpp>

using namespace osg;
using namespace cvr;
using namespace physx;
using namespace osgPhysx;

class mvpCallback:public osg::UniformCallback{
public:
    virtual void operator()(Uniform *uf, NodeVisitor *nv){
        uf->set(Matrixf(glm::value_ptr(ARcoreHelper::instance()->getMVPMatrix())));
        uf->dirty();
    }
};

void UpdateActorCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::MatrixTransform* mt = (node->asTransform() ? node->asTransform()->asMatrixTransform() : NULL);
    if ( mt && _actor )
    {
        PxMat44 matrix( _actor->getGlobalPose() );
        mt->setMatrix( physX2OSG_Rotation(matrix) );
    }
    traverse( node, nv );
}
void PhysxBall::createPlane(osg::Group* parent, osg::Vec3f pos) {
    PxMaterial* mMaterial = _phyEngine->getPhysicsSDK()->createMaterial(0.1, 0.2, 0.5);
    PxTransform pose = PxTransform(PxVec3(.0f, pos.z() * ConfigManager::UNIT_ALIGN_FACTOR, .0f),
                                   PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
    PxRigidStatic* plane = PxCreateStatic(*_phyEngine->getPhysicsSDK(), pose, PxPlaneGeometry(), *mMaterial);
    _phyEngine->addActor("main", plane);

//    addBoard(parent, pos, Vec3f(1.0f, .0f,.0f), PI_2f);
}

void PhysxBall::preFrame() {
    _phyEngine->update();

//    if(_planeTurnedOn){
//        _planeTurnedOn = false;
//        std::vector<osg::Vec3f> planes = ARcoreHelper::instance()->getPlaneCenters();
//        if(planes.size()){
//            float x=0, y=0, z=0;
//            for(int i=0;i<planes.size(); i++){
//                x+=planes[i][0]; x+=planes[i][1];x+=planes[i][2];
//            }
//            createPlane(_scene, osg::Vec3f(x/planes.size(), y/planes.size(), z/planes.size()));
//            for(int i=0;i<5;i++)
//                createBall(_scene, osg::Vec3(x/planes.size() + std::rand()%10 * 0.01f-0.05f, y/planes.size() + std::rand()%10 * 0.1f, 0.5), z/planes.size() + 0.01f);
//        }
//
//    }

}
//void PhysxBall::postFrame() {
    //use ar controller to render
//    const float* pointCloudData;
//    int32_t num_of_points = 0;
//    if(!cvr::ARcoreHelper::instance()->getActiveState())
//        return;
//
//    cvr::ARcoreHelper::instance()->getPointCloudData(pointCloudData, num_of_points);
//    if(!pointCloudData)
//        return;
//    _pointcloudDrawable->updateVertices(pointCloudData, num_of_points);
//    _pointcloudDrawable->updateARMatrix(cvr::ARcoreHelper::instance()->getMVPMatrix());
//}

void PhysxBall::initMenuButtons() {
    _addButton = new MenuButton("Add Ball");
    _addButton->setCallback(this);
    _mainMenu->addItem(_addButton);

    _pointButton = new MenuButton("Show PointCloud");
    _pointButton->setCallback(this);
    _mainMenu->addItem(_pointButton);

    _planeButton = new MenuButton("Do Plane Detection");
    _planeButton->setCallback(this);
    _mainMenu->addItem(_planeButton);

    _addAndyButton = new MenuButton("Add a Cow");
    _addAndyButton->setCallback(this);
    _mainMenu->addItem(_addAndyButton);

    _delAndyButton = new MenuButton("Remove a Cow");
    _delAndyButton->setCallback(this);
    _mainMenu->addItem(_delAndyButton);
}
bool PhysxBall::init() {
    // --------------- create the menu ---------------
    _mainMenu = new SubMenu("PhysxBall", "PhysxBall");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);

    initMenuButtons();

    //--------------init scene node--------------
    _menu = new Group;
    _scene = new Group;


    //--------------init physx-------------------
    _phyEngine = Engine::instance();
    if(!_phyEngine->init())
        return false;
    _phyEngine->addScene("main");
    _assetHelper = new assetLoader(_asset_manager);
    _planeTurnedOn = ARcoreHelper::instance()->getPlaneStatus();
    createPlane(_scene, Vec3f(.0f, .0f, -500.0f));


    SceneManager::instance()->getSceneRoot()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    Vec3f boardPos = Vec3f(0, 10.0f, 0);
    //TEST: NO USE TEMPORATELY
//    createPointCloud(SceneManager::instance()->getSceneRoot());
//    createBall(_scene, osg::Vec3(.0f, 0.5, 0.5), 0.01f);
//    createObject(_scene, Vec3f(.0f, 0.5f,.0f));
//    addBoard(_menu, boardPos);
//    createText(_menu, boardPos + Vec3f(0.1f,-2,0.1f));

    //bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds
    rootSO= new SceneObject("myPluginRoot", false, false, false, false, false);
    sceneSO= new SceneObject("myPluginScene", false, false, false, false, false);
    menuSo= new SceneObject("myPluginMenu", false, false, false, false, false);

    rootSO->addChild(sceneSO);
    rootSO->addChild(menuSo);
    sceneSO->addChild(_scene);
    menuSo->addChild(_menu);

//    rootSO->addChild(_root);
    PluginHelper::registerSceneObject(rootSO, "PhysxBallSceneObjset");
    rootSO->dirtyBounds();
    rootSO->attachToScene();

    return true;
}

void PhysxBall::menuCallback(cvr::MenuItem *item) {
    if(item == _addButton)
        createBall(_scene, Vec3f(.0f,.0f,.0f), 0.01f);
//        createBall(_scene, osg::Vec3(std::rand() % 10 * 0.05f-0.25f, std::rand() % 10 * 0.1f, 0.5), 0.01f);
    else if(item == _pointButton)
        ARcoreHelper::instance()->changePointCloudStatus();
    else if(item == _planeButton){
        ARcoreHelper::instance()->turnOnPlane();
        _planeTurnedOn = true;
    }
    else if(item == _addAndyButton)
        createObject(_scene, Vec3f(.0f,.0f,.0f));

}
void PhysxBall::createPointCloud(osg::Group *parent) {
//    _pointcloudDrawable = new pointDrawable();
//    parent->addChild(_pointcloudDrawable->createDrawableNode(_assetHelper, &_glStateStack));
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

void PhysxBall::addBoard(Group* parent, osg::Vec3f pos, osg::Vec3f rotAxis, float rotAngle) {
    float boardWidth = 50, boardHeight = 50;

    ref_ptr<MatrixTransform> nodeTrans = new MatrixTransform();
    Matrixf transMat;
    transMat.makeTranslate(-boardWidth/2, 0, -boardHeight/2);
    transMat.makeTranslate(pos);
    osg::Geode * geode = new osg::Geode();

    geode->addDrawable(
            _makeQuad(boardWidth, boardHeight, Vec4f(.0f,.0f,.0f,1.0f), Vec3f(.0f,.0f,.0f)));
    if(rotAngle){
        Matrixf rotMat;
        rotMat.makeRotate(rotAngle, rotAxis);
        nodeTrans->setMatrix(rotMat * transMat);
    } else
        nodeTrans->setMatrix(transMat);
    nodeTrans->addChild(geode);
    parent->addChild(nodeTrans);
}

void PhysxBall::createObject(osg::Group *parent, osg::Vec3f pos) {
    osg::ref_ptr<osg::MatrixTransform> objectTrans = new MatrixTransform;

    std::string fhead(getenv("CALVR_RESOURCE_DIR"));
    osg::ref_ptr<Node> objNode = osgDB::readNodeFile(fhead + "models/cow.osgt");

    ref_ptr<Texture2D> objTexture = new Texture2D();
    objTexture->setTextureSize(768,512);
    objTexture->setFilter(osg::Texture::FilterParameter::MIN_FILTER, osg::Texture::FilterMode::LINEAR_MIPMAP_LINEAR);
    objTexture->setFilter(osg::Texture::FilterParameter::MAG_FILTER, osg::Texture::FilterMode::LINEAR);
    objTexture->setWrap(Texture::WRAP_T, Texture::REPEAT);
    objTexture->setWrap(Texture::WRAP_S, Texture::REPEAT);
    objTexture->setImage(osgDB::readImageFile(fhead+"models/cow_skin.jpg"));

    ///////use shader
    Program * program =_assetHelper->createShaderProgramFromFile("shaders/lightingOSG_test.vert","shaders/lightingOSG.frag");
    osg::StateSet * stateSet = objNode->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess",
                                           64.0f) );
    stateSet->addUniform( new osg::Uniform("lightPosition",
                                           osg::Vec3(0,0,1)));
    stateSet->addUniform(new osg::Uniform("uScale", 0.01f));

    Uniform * mvpUniform = new Uniform(Uniform::FLOAT_MAT4, "uarMVP");
    mvpUniform->setUpdateCallback(new mvpCallback);
    stateSet->addUniform(mvpUniform);

    //uModel
    Uniform * modelUniform = new Uniform(Uniform::FLOAT_MAT4, "uModel");
    modelUniform->set(Matrixf(glm::value_ptr(glm::translate(glm::mat4(),
                                                            ARcoreHelper::instance()->getRandomPointPos()))) );
    stateSet->addUniform(modelUniform);
    objectTrans->addChild(objNode.get());
    parent->addChild(objectTrans.get());
}

void PhysxBall::createBall(osg::Group* parent,osg::Vec3f pos, float radius) {
//    PxReal density = 1.0f;
//    PxMaterial* mMaterial = _phyEngine->getPhysicsSDK()->createMaterial(0.1,0.2,0.5);
////    mMaterial->setRestitution(1.0f);
////    mMaterial->setStaticFriction(0);
//    PxSphereGeometry geometrySphere(radius);
//    PxTransform transform(PxVec3(pos.x(), pos.z(), -pos.y()), PxQuat(PxIDENTITY()));
//    PxRigidDynamic *actor = PxCreateDynamic(* _phyEngine->getPhysicsSDK(),
//                                            transform,
//                                            geometrySphere,
//                                            *mMaterial,
//                                            density);
//    if(!actor) return;
//
//    actor->setLinearVelocity(PxVec3(.0f, .0f ,0));
//    actor->setSleepThreshold(0.0);
//    _phyEngine->addActor("main", actor);

    osg::ref_ptr<osg::MatrixTransform> sphereTrans = addSphere(parent, pos, radius);
//    sphereTrans->addUpdateCallback(new UpdateActorCallback(actor));
}
ref_ptr<MatrixTransform> PhysxBall::addSphere(osg::Group*parent, osg::Vec3 pos, float radius)
{
//    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable();
//    shape->setShape(new osg::Sphere(Vec3f(.0,.0,.0), radius));
//    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
//    shape->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
//    osg::ref_ptr<osg::Geode> node = new osg::Geode;

    std::string fhead(getenv("CALVR_RESOURCE_DIR"));
    osg::ref_ptr<Node> node = osgDB::readNodeFile(fhead + "models/box.osgt");

    ///////use shader
    Program * program =_assetHelper->createShaderProgramFromFile("shaders/lightingOSG_test.vert","shaders/lightingOSG.frag");
    osg::StateSet * stateSet = node->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess",
                                           64.0f) );
    stateSet->addUniform( new osg::Uniform("lightPosition",
                                           osg::Vec3(0,0,1)));
    stateSet->addUniform(new osg::Uniform("uScale", 0.01f));

    Uniform * mvpUniform = new Uniform(Uniform::FLOAT_MAT4, "uarMVP");
    mvpUniform->setUpdateCallback(new mvpCallback);
    stateSet->addUniform(mvpUniform);

    //uModel
    Uniform * modelUniform = new Uniform(Uniform::FLOAT_MAT4, "uModel");
    modelUniform->set(Matrixf(glm::value_ptr(glm::translate(glm::mat4(),
                                                            ARcoreHelper::instance()->getRandomPointPos()))) );
    stateSet->addUniform(modelUniform);

//    node->addDrawable(shape.get());
    ref_ptr<MatrixTransform> sphereTrans = new MatrixTransform;
//    Matrixf m;
//    m.makeTranslate(pos);
//    sphereTrans->setMatrix(m);
    sphereTrans->addChild(node.get());

    parent->addChild(sphereTrans.get());
    return sphereTrans.get();
}