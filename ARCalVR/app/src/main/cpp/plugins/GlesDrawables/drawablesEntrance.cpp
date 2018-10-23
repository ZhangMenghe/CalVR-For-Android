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

//    SceneManager::instance()->getSceneRoot()->addChild(_root);
    _pointcloudDrawable = new pointDrawable;
    _root->addChild(_pointcloudDrawable->createDrawableNode(&_glStateStack));
    return true;
}

void GlesDrawables::menuCallback(cvr::MenuItem *item) {
}

void GlesDrawables::preFrame() {
    _pointcloudDrawable->updateOnFrame();
}