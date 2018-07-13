//
// Created by menghe on 7/12/2018.
//

#include "osgController.h"
#include "utils.h"
#include "arcore_utils.h"

#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
using namespace osg_controller;
using namespace osg;
osg::ref_ptr<osg::Node> osgController::_createNode() {
//创建一个叶节点对象
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    //创建一个几何体对象
    osg::ref_ptr<osg::Geometry> geom = new osg::Geometry();
    //添加顶点数据 注意顶点的添加顺序是逆时针
    osg::ref_ptr<osg::Vec3Array> v = new osg::Vec3Array();
    //添加数据
    v->push_back(osg::Vec3(0, 0, 0));
    v->push_back(osg::Vec3(1, 0, 0));
    v->push_back(osg::Vec3(1, 0, 1));
    v->push_back(osg::Vec3(0, 0, 1));

    //设置顶点数据
    geom->setVertexArray(v.get());

    //创建纹理订点数据
    osg::ref_ptr<osg::Vec2Array> vt = new osg::Vec2Array();
    //添加纹理坐标
    vt->push_back(osg::Vec2(0, 0));
    vt->push_back(osg::Vec2(1, 0));
    vt->push_back(osg::Vec2(1, 1));
    vt->push_back(osg::Vec2(0, 1));

    //设置纹理坐标
    geom->setTexCoordArray(0, vt.get());

    //创建颜色数组
    osg::ref_ptr<osg::Vec4Array> vc = new osg::Vec4Array();
    //添加数据
    vc->push_back(osg::Vec4(1, 0, 0, 1));
    vc->push_back(osg::Vec4(0, 1, 0, 1));
    vc->push_back(osg::Vec4(0, 0, 1, 1));
    vc->push_back(osg::Vec4(1, 1, 0, 1));

    //设置颜色数组
    geom->setColorArray(vc.get());
    //设置颜色的绑定方式为单个顶点
    geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

    //创建法线数组
    osg::ref_ptr<osg::Vec3Array> nc = new osg::Vec3Array();
    //添加法线
    nc->push_back(osg::Vec3(0, -1, 0));
    //设置法线
    geom->setNormalArray(nc.get());
    //设置法绑定为全部顶点
    geom->setNormalBinding(osg::Geometry::BIND_OVERALL);
    //添加图元
    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    //添加到叶子节点
    geode->addDrawable(geom.get());

    return geode.get();
}
osgController::osgController(AAssetManager * manager)
        :_asset_manager(manager) {
    _viewer = new Viewer();
    // use single thread: critical for mobile and web
    _viewer->setThreadingModel(ViewerBase::SingleThreaded);
    _viewer->setCameraManipulator(new osgGA::TrackballManipulator());
    _root = new Group;
    //TODO: NO idea why can not add realize
    //_viewer->realize();

    _viewer->addEventHandler(new osgViewer::StatsHandler);
    _viewer->addEventHandler(
            new osgGA::StateSetManipulator(
                    _viewer->getCamera()->getOrCreateStateSet()));
    _viewer->addEventHandler(new osgViewer::ThreadingHandler);
    _viewer->addEventHandler(new osgViewer::LODScaleHandler);

    _manipulator = new osgGA::KeySwitchMatrixManipulator;
    _manipulator->addMatrixManipulator('1', "Trackball",
                                       new osgGA::TrackballManipulator());
    _viewer->setCameraManipulator(_manipulator.get());

    _viewer->getViewerStats()->collectStats("scene", true);
//    _camera_renderer = new osg_cameraRenderer();
}

osgController::~osgController() {
    if(_ar_session) {
        ArSession_destroy(_ar_session);
        ArFrame_destroy(_ar_frame);
    }
    delete(_viewer);
}

void osgController::onCreate() {

    /*bg_cam->setRenderOrder(osg::Camera::PRE_RENDER);
    bg_cam->setReferenceFrame(osg::Camera::ABSOLUTE_RF);
    bg_cam->setViewMatrix(osg::Matrix::identity());
// set the projection matrix to be of width and height of 1
    bg_cam->setProjectionMatrix(osg::Matrix::ortho2D(0, 1.0f, 0, 1.0f));
*/


//    _viewer->getCamera()->setClearMask(GL_DEPTH_BUFFER_BIT);
//    _viewer->getCamera()->setClearColor(Vec4f(1.0f, .0f, .0f, 1.0f));
//    _viewer->getCamera()->setProjectionMatrixAsPerspective(60.0f, 1.33333, 0.01, 100.0);
//
//    _viewer->setUpViewInWindow(11, 11, 800 + 11, 600 + 11);
//
//    _camera_renderer->Initialization(_asset_manager, _root);
//
//    _root->addChild(_camera_renderer->GetGeode());
//   // _root->addChild(bg_cam);

    osg::ref_ptr<osg::Node> debugNode = _createNode();
    debugNode->getOrCreateStateSet()->setAttribute(
            osg_utils::createShaderProgram("shaders/naiveOSG.vert", "shaders/naiveOSG.frag", _asset_manager));
    _root->addChild(debugNode);

    osgViewer::Viewer::Windows windows;
    _viewer->getWindows(windows);
    for (osgViewer::Viewer::Windows::iterator itr = windows.begin();
         itr != windows.end(); ++itr) {
        (*itr)->getState()->setUseModelViewAndProjectionUniforms(true);
        (*itr)->getState()->setUseVertexAttributeAliasing(true);
    }

    _viewer->setSceneData(NULL);
    _viewer->setSceneData(_root.get());
    _manipulator->getNode();
    _viewer->home();

    _viewer->getDatabasePager()->clear();
    _viewer->getDatabasePager()->registerPagedLODs(_root.get());
    _viewer->getDatabasePager()->setUpThreads(3, 1);
    _viewer->getDatabasePager()->setTargetMaximumNumberOfPageLOD(2);
    _viewer->getDatabasePager()->setUnrefImageDataAfterApplyPolicy(true, true);
}

void osgController::onViewChanged(int rot, int width, int height) {
    _viewer->setUpViewerAsEmbeddedInWindow(0,0,width,height);
    _displayRotation = rot;
    _width = width;
    _height = height;
    if (_ar_session != nullptr) {
        ArSession_setDisplayGeometry(_ar_session, _displayRotation, width, height);
    }
}

void osgController::onPause() {
    if(_ar_session)
        ArSession_pause(_ar_session);
}
void osgController::onResume(void *env, void *context, void *activity) {
   // _context = (GraphicsContext *)context;
    if(nullptr == _ar_session){
        ArInstallStatus install_status;
        bool user_requested_install = !_install_requested;

        CHECK(ArCoreApk_requestInstall(env, activity, user_requested_install,&install_status) == AR_SUCCESS);

        switch (install_status) {
            case AR_INSTALL_STATUS_INSTALLED:
                break;
            case AR_INSTALL_STATUS_INSTALL_REQUESTED:
                _install_requested = true;
                return;
        }
        CHECK(ArSession_create(env, context, &_ar_session)==AR_SUCCESS);
        CHECK(_ar_session);
        ArFrame_create(_ar_session, &_ar_frame);
        CHECK(_ar_frame);
        ArSession_setDisplayGeometry(_ar_session, _displayRotation, _width, _height);
    }
    const ArStatus status = ArSession_resume(_ar_session);
    CHECK(status == AR_SUCCESS);
}
void osgController::onDrawFrame(bool btn_status_normal) {

    //if(_ar_session == nullptr)
    //    return;
    //must call this func before update ar session
    //TODO:FATAL ERROR HERE
    /*ArSession_setCameraTextureName(_ar_session,_camera_renderer->GetTextureId(_context));
    // Update session to get current frame and render camera background.
    if (ArSession_update(_ar_session, _ar_frame) != AR_SUCCESS) {
        LOGE("OnDrawFrame ArSession_update error");
    }
    ArCamera* camera;
    ArFrame_acquireCamera(_ar_session, _ar_frame, &camera);

    mat4 view_mat;
    mat4 proj_mat;

    ArCamera_getViewMatrix(_ar_session, camera, value_ptr(view_mat));
    ArCamera_getProjectionMatrix(_ar_session,camera, 0.1f, 100.0f, value_ptr(proj_mat));

    ArTrackingState cam_track_state;
    ArCamera_getTrackingState(_ar_session, camera, &cam_track_state);
    ArCamera_release(camera);*/

    //_camera_renderer->Draw(_ar_session, _ar_frame, btn_status_normal);
    //not tracking anything
   /* if(cam_track_state != AR_TRACKING_STATE_TRACKING)
        return;*/
    _viewer->frame();
}
void osgController::onTouched(float x, float y) {}
void osgController::debug_loadScene(const char *filename) {
    osg::Node *scene = osgDB::readNodeFile(filename);
    if(nullptr == scene){
            LOGE("FAILED TO LOAD SCENE");
            return;
    }
    scene->getOrCreateStateSet()->setAttribute(
                osg_utils::createShaderProgram("shaders/naiveOSG.vert", "shaders/naiveOSG.frag", _asset_manager));
    _root->addChild(scene);
}
