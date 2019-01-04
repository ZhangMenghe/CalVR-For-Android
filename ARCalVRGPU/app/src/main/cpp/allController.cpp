//#include <cvrUtil/AndroidStdio.h>
#include "allController.h"
//#include <cvrUtil/AndroidHelper.h>
//#include <osg/ShapeDrawable>
//#include <cvrUtil/AndroidPreloadPlugins.h>
//#include <MenuBasics.h>
//#include <PhysxBall.h>
//#include <drawablesEntrance.h>
////#include <SpatialViz.h>
//#include <osg/Depth>
//#include <osg/Callback>
//#include <cvrKernel/PluginManager.h>
//
//using namespace cvr;
//using namespace osg;
//
//REGISTER(MenuBasics);
//REGISTER(PhysxBall)
//REGISTER(GlesDrawables);
////REGISTER(SpatialViz);
//
//void JNICallBackCallback::operator()(osg::Node *node, osg::NodeVisitor *nv) {
//    std::string functionName;
//    if(PluginManager::getCallBackRequest(functionName)){
//        if(_map.find(functionName) != _map.end()){
//            if(_map[functionName].funMode == 0)
//                _env->CallVoidMethod(_obj, _map[functionName].id);
//            else if(_map[functionName].funMode == 1){
//                sizeArr.clear();
//                jboolean copy = 0;
//                jfloatArray sizeArray = (jfloatArray)_env->CallObjectMethod(_obj, _map[functionName].id);
//                jsize len = _env->GetArrayLength(sizeArray);
//                jfloat * content = _env->GetFloatArrayElements(sizeArray, &copy);
//                for(int i=0;i<len;i++)
//                    sizeArr.push_back(content[i]);
//            }
//
//        }
//    }
//    traverse( node, nv );
//}
//
//void JNICallBackCallback::registerCallBackFunction(std::string funcName, const char* signature, int funMode){
//    jmethodID method = _env->GetMethodID(_helper_class, funcName.c_str(), signature);
//    if(_map.find(funcName) == _map.end()){
//        mFun function;
//        function.id = method;
//        function.funMode = funMode;
//        _map[funcName] = function;
//    }
//}

allController::allController(AAssetManager *assetManager)
        :_asset_manager(assetManager){
//    _CalVR = new CalVR();
//    _root = nullptr;
//    _sceneGroup = nullptr;
//    _bgDrawable = new bgDrawable();
    _fpsMonitor = new perfMonitor();
    _stitcher = cv::Stitcher::createDefault();
}

allController::~allController(){
//    delete _CalVR;
}

void allController::onCreate(const char * calvr_path){
//    if(!_CalVR->init(calvr_path, _asset_manager))
//        LOGE("Failed to init calvr kernel");
//
//    _root = new Group;
//    _sceneGroup = new Group;
////    _bgDrawable->createDrawableNode();
////    _root->addChild(_bgDrawable->createDrawableNode());
////    ARCoreManager::instance()->setCameraTextureTarget(_bgDrawable->GetTextureId());
//    _root->addChild(_sceneGroup);
//    _sceneGroup->addChild(_CalVR->getSceneRoot());
////    _sceneGroup->addChild(createDebugOSGSphere(Vec3f(.0f, 0.5f, .0f)));
////    _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
////    _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
////    _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
//    _CalVR->setSceneData(_root.get());
//
//    //Initialize callback env
//    JNIEnv* env = GetJniEnv();
//    jclass helper_class = env->FindClass( "com/samsung/arcalvr/MainActivity" );
//    if(helper_class){
//        helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
//        JNIcallback = new JNICallBackCallback(env, helper_class, GetMainActivityObj());
//        JNIcallback->registerCallBackFunction("popButtons", "()V");
////        JNIcallback->registerCallBackFunction("getPixelSize", "()[F", 1);
//        _sceneGroup->addUpdateCallback(JNIcallback);
//    }
}

void allController::onPause(){
//    _CalVR->onPause();
}

void allController::onResume(){
//    _CalVR->onResume(env, context, activity);
//    PluginManager::setCallBackRequest("getPixelSize");
}

void allController::onDrawFrame(){
//    _CalVR->frame();
//    _bgDrawable->updateOnFrame(ARCoreManager::instance()->getCameraTransformedUVs());
//
//    if(_detectStart){
//        //shoot the ray to check the interaction with menu
//        AndroidInteractionEvent * aie = new AndroidInteractionEvent();
//        aie->setInteraction(BUTTON_DRAG);
//        _CalVR->setTouchEvent(aie, LEFT, _touchX, _touchY);
////        TrackingManager::instance()->setTouchMovePosition(_touchX,_touchY);
//    }
}

void allController::onViewChanged(int rot, int width, int height){
    _touchX = width/2; _touchY = height/2;
//    _CalVR->onViewChanged(rot, width, height);
}
//
//void allController::onSingleTouchDown(TouchType type, float x, float y){
//    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
//    aie->setInteraction(BUTTON_DOWN);
//    _CalVR->setTouchEvent(aie, type, x, y);
//}
//
//void allController::onSingleTouchUp(TouchType type, float x, float y){
//    if(type == RIGHT) _detectStart = false;
//    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
//    aie->setInteraction(BUTTON_UP);
//    _CalVR->setTouchEvent(aie, type, x, y);
//}
//
//void allController::onDoubleTouch(TouchType type, float x, float y){
//    if(type==RIGHT) _detectStart = true;
//    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
//    aie->setInteraction(BUTTON_DOUBLE_CLICK);
//    _CalVR->setTouchEvent(aie, type, x, y);
//}
//
//void allController::onTouchMove(TouchType type, float x, float y){
//
//    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
//    aie->setInteraction(BUTTON_DRAG);
//    _CalVR->setTouchEvent(aie, type, x, y);
//}
//void allController::setPixelSize(float * arr){
//    ARCoreManager::instance()->setPixelSize(arr[0], arr[1]);
//}
//ref_ptr<osg::Geode> allController::createDebugOSGSphere(osg::Vec3 pos) {
//    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
//    shape->setShape(new osg::Sphere(pos, 0.05f));
//    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
//    osg::ref_ptr<osg::Geode> node = new osg::Geode;
//    Program * program = assetLoader::instance()->createShaderProgramFromFile("shaders/lighting.vert","shaders/lighting.frag");
//
//    osg::StateSet * stateSet = shape->getOrCreateStateSet();
//    stateSet->setAttributeAndModes(program);
//
//    stateSet->addUniform( new osg::Uniform("lightDiffuse",
//                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
//    stateSet->addUniform( new osg::Uniform("lightSpecular",
//                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
//    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );
//
//    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));
//
//    Uniform * baseColor = new osg::Uniform("uBaseColor", osg::Vec4f(1.0f, .0f, .0f, 1.0f));
//    stateSet->addUniform(baseColor);
//
//    node->addDrawable(shape.get());
//
//    return node;
//}
void allController::processFrameImage(cv::Mat image){
    cv::Stitcher::Status status = _stitcher.stitch(_stichImages, _panoImg);
    if(status == cv::Stitcher::OK || _stichImages.size() > 10)
        _stichImages.clear();
    else {
        cv::resize(image, image, cv::Size(image.rows/2, image.cols/2));
        _stichImages.push_back(image);
    }
}