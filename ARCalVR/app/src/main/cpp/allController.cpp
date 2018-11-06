#include <cvrUtil/AndroidStdio.h>
#include "allController.h"
#include <cvrUtil/AndroidHelper.h>
#include <osg/ShapeDrawable>
#include <cvrUtil/AndroidPreloadPlugins.h>
#include <MenuBasics.h>
#include <PhysxBall.h>
#include <drawablesEntrance.h>
#include <osg/Depth>
#include <osg/Callback>
#include <cvrKernel/PluginManager.h>

using namespace cvr;
using namespace osg;

REGISTER(MenuBasics);
REGISTER(PhysxBall)
REGISTER(GlesDrawables);

void JNICallBackCallback::operator()(osg::Node *node, osg::NodeVisitor *nv) {
    std::string functionName;
    if(PluginManager::getCallBackRequest(functionName)){
        jmethodID method = _env->GetMethodID(_helper_class, functionName.c_str(), "()V");
        _env->CallVoidMethod(_obj, method);
    }
    traverse( node, nv );
}

allController::allController(AAssetManager *assetManager)
        :_asset_manager(assetManager){
    _CalVR = new CalVR();
    _root = nullptr;
    _sceneGroup = nullptr;
    _bgDrawable = new bgDrawable();
}

allController::~allController(){
    delete _CalVR;
}

void allController::onCreate(const char * calvr_path){
    if(!_CalVR->init(calvr_path, _asset_manager))
        LOGE("Failed to init calvr kernel");

    _root = new Group;
    _sceneGroup = new Group;

    _root->addChild(_bgDrawable->createDrawableNode());
    ARCoreManager::instance()->setCameraTextureTarget(_bgDrawable->GetTextureId());
    _root->addChild(_sceneGroup);
    _sceneGroup->addChild(_CalVR->getSceneRoot());
//    _sceneGroup->addChild(createDebugOSGSphere(Vec3f(.0f, 0.5f, .0f)));
//    _sceneGroup->getOrCreateStateSet()->setRenderBinDetails(2, "RenderBin");
//    _sceneGroup->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::ON);
//    _root->getOrCreateStateSet()->setMode(GL_DEPTH_TEST,osg::StateAttribute::OFF);
    _CalVR->setSceneData(_root.get());

    JNIEnv* env = GetJniEnv();
    jclass helper_class = env->FindClass( "com/samsung/arcalvr/MainActivity" );
    if(helper_class){
        helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
        _sceneGroup->addUpdateCallback(new JNICallBackCallback(env, helper_class, GetMainActivityObj()));
    }
}

void allController::onPause(){
    _CalVR->onPause();
}

void allController::onResume(void * env, void* context, void* activity){
    _CalVR->onResume(env, context, activity);
}

void allController::onDrawFrame(){
    _CalVR->frame();
    _bgDrawable->updateOnFrame(ARCoreManager::instance()->getCameraTransformedUVs());

    if(_detectStart){
        //shoot the ray to check the interaction with menu
        AndroidInteractionEvent * aie = new AndroidInteractionEvent();
        aie->setInteraction(BUTTON_DRAG);
        _CalVR->setTouchEvent(aie, LEFT, _touchX, _touchY);
        TrackingManager::instance()->setTouchMovePosition(_touchX,_touchY);
    }
}

void allController::onViewChanged(int rot, int width, int height){
    _touchX = width/2; _touchY = height/2;
    _CalVR->onViewChanged(rot, width, height);
    LOGE("===%d, %d", width, height);
}

void allController::onSingleTouchDown(TouchType type, float x, float y){
    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
    aie->setInteraction(BUTTON_DOWN);
    _CalVR->setTouchEvent(aie, type, x, y);
}

void allController::onSingleTouchUp(TouchType type, float x, float y){
    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
    aie->setInteraction(BUTTON_UP);
    _CalVR->setTouchEvent(aie, type, x, y);
}

void allController::callJavaTest(const char* funcName){
    JNIEnv* env = GetJniEnv();
    jclass helper_class = env->FindClass( "com/samsung/arcalvr/MainActivity" );
    if(helper_class){
        helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
        jmethodID button_move_method = env->GetMethodID(helper_class, funcName, "()V");
        jobject object = GetMainActivityObj();
        env->CallVoidMethod(object,button_move_method);
    }

        /*
         * [ capture clause ] (parameters) -> return-type
{
   definition of method
}
         */
//    static struct JNIData{
//        jclass helper_class;
//        jmethodID test_method;
//        jmethodID button_move_method;
//    }jniIds = [env]()->JNIData{
//        constexpr char kHelperClassName[] = "com/samsung/arcalvr/JniInterface";
//        constexpr char kTestFunctionName[] = "testCallBack";
//        constexpr char kButtonMoveFunction[]="popButtons";
//        constexpr char kSignature[] = "()V";
//
//        jclass helper_class = env->FindClass( kHelperClassName );
//
//        if(helper_class){
//            helper_class = static_cast<jclass>(env->NewGlobalRef(helper_class));
//            jmethodID test_method = env->GetStaticMethodID(
//                    helper_class, kTestFunctionName, kSignature);
//            jmethodID button_move_method = env->GetMethodID(
//                    helper_class, kButtonMoveFunction, kSignature);
//            return {helper_class, test_method, button_move_method};
//        }
//        LOGE("===CAN'T FIND HELPER CLASS");
//        return {};
//    }();
//    if(!jniIds.helper_class)
//        return;
//    jmethodID constructor = env->GetMethodID(jniIds.helper_class, "<init>", "()V");
//    env->CallStaticVoidMethod(jniIds.helper_class, jniIds.test_method);
//    jobject object = env->NewObject(jniIds.helper_class, constructor);
//    env->CallVoidMethod(object,jniIds.button_move_method);
}
void allController::onDoubleTouch(TouchType type, float x, float y){
    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
    aie->setInteraction(BUTTON_DOUBLE_CLICK);
    _CalVR->setTouchEvent(aie, type, x, y);

    ///for debug
//    callJavaTest("popButtons");
//    if(_callbackHelper)
//        _callbackHelper->callFunction("popButtons");
}

void allController::onTouchMove(TouchType type, float x, float y){
    _detectStart = true;
    _touchX = x; _touchY = y;
    AndroidInteractionEvent * aie = new AndroidInteractionEvent();
    aie->setInteraction(BUTTON_DRAG);
    _CalVR->setTouchEvent(aie, type, x, y);
}

float allController::getFPS(){return 60.0f;}

ref_ptr<osg::Geode> allController::createDebugOSGSphere(osg::Vec3 pos) {
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
    shape->setShape(new osg::Sphere(pos, 0.05f));
    shape->setColor(osg::Vec4f(1.0f,.0f,.0f,1.0f));
    osg::ref_ptr<osg::Geode> node = new osg::Geode;
    Program * program = assetLoader::instance()->createShaderProgramFromFile("shaders/lighting.vert","shaders/lighting.frag");

    osg::StateSet * stateSet = shape->getOrCreateStateSet();
    stateSet->setAttributeAndModes(program);

    stateSet->addUniform( new osg::Uniform("lightDiffuse",
                                           osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("lightSpecular",
                                           osg::Vec4(1.0f, 1.0f, 0.4f, 1.0f)) );
    stateSet->addUniform( new osg::Uniform("shininess", 64.0f) );

    stateSet->addUniform( new osg::Uniform("lightPosition", osg::Vec3(0,0,1)));

    Uniform * baseColor = new osg::Uniform("uBaseColor", osg::Vec4f(1.0f, .0f, .0f, 1.0f));
    stateSet->addUniform(baseColor);

    node->addDrawable(shape.get());

    return node;
}