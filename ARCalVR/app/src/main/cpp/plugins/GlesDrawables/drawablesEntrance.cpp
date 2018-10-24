#include "drawablesEntrance.h"
using namespace osg;
using namespace cvr;

void GlesDrawables::initMenuButtons() {
    _pointButton = new MenuButton("Show PointCloud");
    _pointButton->setCallback(this);
    _mainMenu->addItem(_pointButton);

    _planeButton = new MenuButton("Show Detected Planes");
    _planeButton->setCallback(this);
    _mainMenu->addItem(_planeButton);

    _strokeButton = new MenuButton("Show Stroke Ray");
    _strokeButton->setCallback(this);
    _mainMenu->addItem(_strokeButton);
}

bool GlesDrawables::init() {
    // --------------- create the menu ---------------
    _mainMenu = new SubMenu("GlesDrawable", "GlesDrawable");
    _mainMenu->setCallback(this);
    MenuSystem::instance()->addMenuItem(_mainMenu);
    initMenuButtons();

    _root = new Group;
    SceneManager::instance()->getSceneRoot()->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    //bool navigation, bool movable, bool clip, bool contextMenu, bool showBounds
    rootSO= new SceneObject("glesRoot", false, false, false, false, false);
    rootSO->addChild(_root);

    PluginHelper::registerSceneObject(rootSO, "GlesDrawablesPlugin");
    rootSO->dirtyBounds();
    rootSO->attachToScene();

    _pointcloudDrawable = new pointDrawable;
    _root->addChild(_pointcloudDrawable->createDrawableNode());
    createObject(_root, Vec3f(.0,.0,-0.1f)); // opengl coordinate
    return true;
}

void GlesDrawables::menuCallback(cvr::MenuItem *item) {
}

void GlesDrawables::preFrame() {
    _pointcloudDrawable->updateOnFrame();
}
void GlesDrawables::createObject(osg::Group *parent, Vec3f pos) {
    osg::ref_ptr<osg::MatrixTransform> objectTrans = new MatrixTransform;

    std::string fhead(getenv("CALVR_RESOURCE_DIR"));
    osg::ref_ptr<Node> objNode = osgDB::readNodeFile(fhead + "models/box.osgt");


    ///////use shader
    Program * program =assetLoader::instance()->createShaderProgramFromFile("shaders/object.vert","shaders/object.frag");
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
    stateSet->addUniform(new osg::Uniform("uScale", 0.1f));

    Uniform * mvpUniform = new Uniform(Uniform::FLOAT_MAT4, "uarMVP");
    mvpUniform->setUpdateCallback(new mvpCallback);
    stateSet->addUniform(mvpUniform);

    //uModel
    Uniform * modelUniform = new Uniform(Uniform::FLOAT_MAT4, "uModel");
    modelUniform->set(Matrixf::translate(pos));
    stateSet->addUniform(modelUniform);
    objectTrans->addChild(objNode.get());
    parent->addChild(objectTrans.get());
}