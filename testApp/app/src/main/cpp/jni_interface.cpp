#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jni_interface.h"
#include <android/bitmap.h>
#include <utility> //pair and make_pair
#include <vector>
#include "dcmRenderer.h"
namespace {
    jobject main_object;
    //global environment
    jlong renderAddr = 0;

    inline jlong getNativeClassAddr(dcmVolumeRender * native_controller){
        return reinterpret_cast<intptr_t>(native_controller);
    }
    inline dcmVolumeRender * renderNative(jlong ptr){
        return reinterpret_cast<dcmVolumeRender *>(ptr);
    }
    typedef struct {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t alpha;
    } argb;
}
void convert_bitmap(JNIEnv* env, jobject bitmap, GLubyte*& data, int&w, int &h ){
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
    data = new GLubyte[size];

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

/*Native Application methods*/
JNI_METHOD(jlong, JNIonCreate)
(JNIEnv* env, jclass, jobject asset_manager){
    AAssetManager * cpp_asset_manager = AAssetManager_fromJava(env, asset_manager);
    renderAddr =  getNativeClassAddr(new dcmVolumeRender(cpp_asset_manager));
    return renderAddr;
}

JNI_METHOD(void, JNIsendDCMImgs)(JNIEnv* env, jobject obj,  jobjectArray img_arr, jint size){
    //get dcmImg class defined in java
    jclass imgClass = env->FindClass("lapras/testapp/dcmImage");
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
        GLubyte * data = nullptr;
        convert_bitmap(env, bitmap, data, width, height);
        renderNative(renderAddr)->addImage(data, location);
    }
    if(last_valid!=-1){
        img = env->GetObjectArrayElement(img_arr, last_valid);
        xspace_id = env->GetFieldID(imgClass, "xSpacing", "F");
        xspacing = env->GetFloatField(img, xspace_id);

        yspace_id = env->GetFieldID(imgClass, "ySpacing", "F");
        yspacing = env->GetFloatField(img, yspace_id);

        GLubyte * data = nullptr;
        convert_bitmap(env, bitmap, data, width, height);

        renderNative(renderAddr)->initDCMIProperty(width * xspacing, height*yspacing, thickness*valid_num);
    }
    renderNative(renderAddr)->assembleTexture();
}
JNI_METHOD(void, JNIonGlSurfaceCreated)
(JNIEnv *, jclass){
    renderNative(renderAddr)->onViewCreated();
}
JNI_METHOD(void, JNIonSurfaceChanged)(JNIEnv * env, jclass, jint w, jint h){
    renderNative(renderAddr)->onViewChange(w, h);
}
JNI_METHOD(void, JNIdrawFrame)(JNIEnv*, jobject){
    renderNative(renderAddr)->onDraw();
}

JNI_METHOD(void, JNIonSingleTouchDown)(JNIEnv * env, jclass,jfloat x, jfloat y){
    renderNative(renderAddr)->onSingleTouchDown(x, y);
}

JNI_METHOD(void, JNIonTouchMove)(JNIEnv * env, jclass, jfloat x, jfloat y){
    renderNative(renderAddr)->onTouchMove(x, y);
}

JNI_METHOD(void, JNIonDoubleTouch)(JNIEnv * env, jclass, jint indicate,jfloat x, jfloat y){
    renderNative(renderAddr)->onDoubleTouch(indicate, x, y);
}