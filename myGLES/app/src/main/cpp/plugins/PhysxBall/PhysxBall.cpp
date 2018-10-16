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
using namespace osgPhysx;

osg::Matrix physX2OSG( const PxMat44& m )
{
    PxVec3 pos = m.getPosition();
    Matrix trans;
    trans.makeTranslate(pos.x, -pos.z, pos.y);

    Matrix rotMat;
    double w = sqrt(1.0 + m(0,0) + m(1,1) + m(2,2)) / 2.0;
    double w4 = (4.0 * w);
    double x = (m(2,1) - m(1,2)) / w4 ;
    double y = (m(0,2) - m(2,0)) / w4 ;
    double z = (m(1,0) - m(0,1)) / w4 ;
    rotMat.makeRotate(Quat(x,-z,y,w));
    return trans;
}

void UpdateActorCallback::operator()( osg::Node* node, osg::NodeVisitor* nv )
{
    osg::MatrixTransform* mt = (node->asTransform() ? node->asTransform()->asMatrixTransform() : NULL);
    if ( mt && _actor )
    {
        PxMat44 matrix( _actor->getGlobalPose() );
        mt->setMatrix( physX2OSG(matrix) );
    }
    traverse( node, nv );
}
void PhysxBall::createPlane(osg::Group* parent, osg::Vec3f pos) {
    PxMaterial* mMaterial = _phyEngine->getPhysicsSDK()->createMaterial(0.1, 0.2, 0.5);
//    mMaterial->setRestitution(1.0f);
//    mMaterial->setStaticFriction(0);
    PxTransform pose = PxTransform(PxVec3(.0f, pos.z(),pos.y()),
                                   PxQuat(PxHalfPi, PxVec3(0.0f, 0.0f, 1.0f)));
//    PxRigidStatic* plane = _phyEngine->getPhysicsSDK()->createRigidStatic(pose);
    PxRigidStatic* plane = PxCreateStatic(*_phyEngine->getPhysicsSDK(), pose, PxPlaneGeometry(), *mMaterial);

//    PxShape *shape = PxRigidActorExt::createExclusiveShape(*plane, PxPlaneGeometry(), *mMaterial);
//    if(!shape) return;
    _phyEngine->addActor("main", plane);

//    addBoard(parent, pos, Vec3f(1.0f, .0f,.0f), PI_2f);
}

void PhysxBall::preFrame() {
    _phyEngine->update();
//    PxRigidActor * ball = static_cast<PxRigidActor * >(_phyEngine->getActorAt("main", 1));
//    PxMat44 matrix( ball->getGlobalPose() );
//    sphereTrans->setMatrix( physX2OSG(matrix) );

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
    _menu = new Group;
    _scene = new Group;

    //--------------init physx-------------------
    _phyEngine = Engine::instance();
    if(!_phyEngine->init())
        return false;
    _phyEngine->addScene("main");

    createPlane(_scene, Vec3f(.0f, .0f, -0.25f));


    SceneManager::instance()->getSceneRoot()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    Vec3f boardPos = Vec3f(0, 10.0f, 0);
    createBall(_scene, osg::Vec3(.0f, 0.5, 0.5), 0.01f);

//    addBoard(_menu, boardPos);
//    createText(_menu, boardPos + Vec3f(0.1f,-2,0.1f));

    //bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds
    rootSO= new SceneObject("myPluginRoot", false, false, false, false, false);
    sceneSO= new SceneObject("myPluginScene", false, false, false, false, true);
    menuSo= new SceneObject("myPluginMenu", false, false, false, false, true);

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
//    rootSO->dirtyBounds();
//    if(item == _addButton)
//        createBall(_balls, Vec3(.0f,0.5f,.0f), 0.05f);
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
void PhysxBall::addBoard(Group* parent, osg::Vec3f pos, osg::Vec3f rotAxis, float rotAngle) {
    float boardWidth = 5, boardHeight = 5;

    ref_ptr<MatrixTransform> nodeTrans = new MatrixTransform();
    Matrixf transMat;
    transMat.makeTranslate(-boardWidth/2, 0, -boardHeight/2);

    osg::Geode * geode = new osg::Geode();

    geode->addDrawable(
            _makeQuad(boardWidth, boardHeight, Vec4f(.0f,.0f,.0f,1.0f), pos));
    if(rotAngle){
        Matrixf rotMat;
        rotMat.makeRotate(rotAngle, rotAxis);
        nodeTrans->setMatrix(transMat * rotMat);
    } else
        nodeTrans->setMatrix(transMat);
    nodeTrans->addChild(geode);
    parent->addChild(nodeTrans);
}

void PhysxBall::createBall(osg::Group* parent,osg::Vec3f pos, float radius) {
    PxReal density = 1.0f;
    PxMaterial* mMaterial = _phyEngine->getPhysicsSDK()->createMaterial(0.1,0.2,0.5);
//    mMaterial->setRestitution(1.0f);
//    mMaterial->setStaticFriction(0);
    PxSphereGeometry geometrySphere(radius);
    PxTransform transform(PxVec3(pos.x(), pos.z(), -pos.y()), PxQuat(PxIDENTITY()));
    PxRigidDynamic *actor = PxCreateDynamic(* _phyEngine->getPhysicsSDK(),
                                            transform,
                                            geometrySphere,
                                            *mMaterial,
                                            density);
//    actor->setAngularDamping(0.75);
    actor->setLinearVelocity(PxVec3(.0f, .0f ,0));
    actor->setSleepThreshold(0.0);
//    actor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
    if(!actor) return;
    _phyEngine->addActor("main", actor);

    osg::ref_ptr<osg::MatrixTransform> sphereTrans = addSphere(parent, pos, radius);
    sphereTrans->addUpdateCallback(new UpdateActorCallback(actor));
}
ref_ptr<MatrixTransform> PhysxBall::addSphere(osg::Group*parent, osg::Vec3 pos, float radius)
{
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable();
    shape->setShape(new osg::Sphere(Vec3f(.0,.0,.0), radius));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    shape->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    node->addDrawable(shape.get());
    ref_ptr<MatrixTransform> sphereTrans = new MatrixTransform;
    Matrixf m;
    m.makeTranslate(pos);
    sphereTrans->setMatrix(m);
    sphereTrans->addChild(node.get());

    parent->addChild(sphereTrans.get());
    return sphereTrans.get();
}