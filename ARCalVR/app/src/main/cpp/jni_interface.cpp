#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/bitmap.h>
#include <cvrUtil/AndroidStdio.h>
#include <cvrUtil/AndroidDCMHelper.h>
#include "jni_interface.h"
#include "allController.h"

namespace {
    //maintain a reference to VM
    static JavaVM *g_vm = nullptr;

    jobject ui_object;
    //global environment
    jlong nativeAppAddr = 0;
    inline allController * controllerNative(jlong ptr){
        return reinterpret_cast<allController *>(ptr);
    }

    typedef struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } argb;

    class dcmImage{
    public:
        unsigned char * data;
        float location;

        dcmImage(unsigned char * _data, float _location):
                data(_data), location(_location){}

    };
    size_t img_width, img_height, dimensions;
    std::vector<dcmImage *> images_;
    unsigned char *data;
}

void convert_bitmap(JNIEnv* env, jobject bitmap, unsigned char*& data, int&w, int &h ){
    AndroidBitmapInfo srcInfo;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_getInfo(env, bitmap, &srcInfo)) {
        LOGE("get bitmap info failed");
        return;
    }
    void * buffer;
    if (ANDROID_BITMAP_RESULT_SUCCESS != AndroidBitmap_lockPixels(env, bitmap, &buffer)) {
        LOGE("lock src bitmap failed");
        return;
    }
    LOGI("width=%d; height=%d; stride=%d; format=%d;flag=%d",
         srcInfo.width, //  width=2700 (900*3)
         srcInfo.height, // height=2025 (675*3)
         srcInfo.stride, // stride=10800 (2700*4)
         srcInfo.format, // format=1 (ANDROID_BITMAP_FORMAT_RGBA_8888=1)
         srcInfo.flags); // flags=0 (ANDROID_BITMAP_RESULT_SUCCESS=0)
    w = srcInfo.width; h = srcInfo.height;

    size_t size = srcInfo.width * srcInfo.height;
    data = new unsigned char[size];

    int x, y, idx = 0;
    for (y = 0; y < h; y++) {
        argb * line = (argb *) buffer;
        for (x = 0; x < w; x++) {
            data[idx++] = line[x].red;
//            LOGE("=== RGBA: %d, %d, %d, %d", line[x].red, line[x].green,line[x].blue, line[x].alpha);
        }

        buffer = (char *) buffer + srcInfo.stride;
    }



    AndroidBitmap_unlockPixels(env, bitmap);
}

void initDCMIProperty(size_t w, size_t h, int thickness){img_width = w; img_height = h;}
void addImage(unsigned char * img, float location){
    images_.push_back(new dcmImage(
            img,
            location));
}
void assembleTexture(){
    std::sort(images_.begin(), images_.end(),
              [](const dcmImage* img1, const dcmImage* img2){return img1->location < img2->location;});
    dimensions = images_.size();
    size_t data_size = img_width * img_height * dimensions;

    data = new unsigned char[data_size];
    auto each_size = img_height * img_width* sizeof( unsigned char);
    for(int i=0; i<dimensions; i++)
        memcpy(data+i*each_size, images_[i]->data, each_size);
    DCMI::img_width = img_width; DCMI::img_height = img_height; DCMI::img_nums = dimensions;
    DCMI::volume_data = data;
}

//JNI Library function: call when native lib is load(System.loadLibrary)
jint JNI_OnLoad(JavaVM *vm, void *) {
    g_vm = vm;
    return JNI_VERSION_1_6;
}

UI_METHOD(void, JUIonUICreated)
(JNIEnv * env, jobject obj){
    ui_object = env->NewGlobalRef(obj);
}

/*Native Application methods*/
JNI_METHOD(jlong, JNIcreateController)
(JNIEnv* env, jclass, jobject asset_manager){

    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    nativeAppAddr =  reinterpret_cast<intptr_t>(new allController(cpp_asset_manager));
    return nativeAppAddr;
}

JNI_METHOD(void, JNIdrawFrame)
(JNIEnv *, jclass) {
    controllerNative(nativeAppAddr)->onDrawFrame();
}

JNI_METHOD(void, JNIonGlSurfaceCreated)
(JNIEnv * env, jclass, jstring calvr_path){
    const char* cpath = env->GetStringUTFChars(calvr_path, JNI_FALSE);
    controllerNative(nativeAppAddr)->onCreate(cpath);
    env->ReleaseStringUTFChars(calvr_path, cpath);
}

JNI_METHOD(void, JNIonViewChanged)
(JNIEnv*, jclass, jint rotation, jint width, jint height){
    controllerNative(nativeAppAddr)->onViewChanged(rotation, width, height);
}

JNI_METHOD(void, JNIonSingleTouchUp)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchUp(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onSingleTouchDown(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onDoubleTouch(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonTouchMove)(JNIEnv*, jclass, jint type, jfloat x, jfloat y){
    controllerNative(nativeAppAddr)->onTouchMove(static_cast<cvr::TouchType>(type), x, y);
}

JNI_METHOD(void, JNIonResume)(JNIEnv * env, jclass, jobject contex, jobject activitiy){
    controllerNative(nativeAppAddr)->onResume(env, contex, activitiy);
}
JNI_METHOD(void, JNIonPause)(JNIEnv *, jclass){
    controllerNative(nativeAppAddr)->onPause();
}
JNI_METHOD(void, JNIonDestroy)(JNIEnv *, jclass, long controller_addr){
    GetJniEnv()->DeleteGlobalRef(ui_object);
    delete controllerNative(controller_addr);
}
JNI_METHOD(jfloat, JNIgetFPS)(JNIEnv *, jclass){
    return controllerNative(nativeAppAddr)->getFPS();
}

JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv* env, jobject obj,  jobjectArray img_arr, jint size){
    //get dcmImg class defined in java
    jclass imgClass = env->FindClass("com/samsung/arcalvr/Utils/dcmImage");
    jobject img, bitmap;
    jfieldID bitmap_id, location_id, thickness_id, xspace_id, yspace_id;
    float location, xspacing, yspacing, thickness;
    int valid_num = 0, last_valid = -1;
    int width, height;
    for(int i=0; i<size; i++) {
        img = env->GetObjectArrayElement(img_arr, i);

        thickness_id = env->GetFieldID(imgClass, "thickness", "F");
        thickness = env->GetFloatField(img, thickness_id);
        if(thickness == -1)//invalid
            continue;
        else{last_valid = i; valid_num++;}

        location_id = env->GetFieldID(imgClass, "location", "F");
        location = env->GetFloatField(img, location_id);

        bitmap_id = env->GetFieldID(imgClass, "bitmap", "Landroid/graphics/Bitmap;");
        bitmap = env->GetObjectField(img,bitmap_id);
        unsigned char * data_tmp = nullptr;
        convert_bitmap(env, bitmap, data_tmp, width, height);
        addImage(data_tmp, location);
    }
    if(last_valid!=-1){
        img = env->GetObjectArrayElement(img_arr, last_valid);
        xspace_id = env->GetFieldID(imgClass, "xSpacing", "F");
        xspacing = env->GetFloatField(img, xspace_id);

        yspace_id = env->GetFieldID(imgClass, "ySpacing", "F");
        yspacing = env->GetFloatField(img, yspace_id);

        unsigned char * data_tmp2 = nullptr;
        convert_bitmap(env, bitmap, data_tmp2, width, height);

        initDCMIProperty(width, height, thickness*valid_num);

//        renderNative(renderAddr)->initDCMIProperty(width * xspacing, height*yspacing, thickness*valid_num);
    }
    assembleTexture();
}

JNIEnv *GetJniEnv() {
    JNIEnv *env;
    jint result = g_vm->AttachCurrentThread(&env, nullptr);
    return result == JNI_OK ? env : nullptr;
}
jobject GetJUIObject(){
    return ui_object;
}