#ifndef ANDROID_HELPER
#define ANDROID_HELPER

#include <string>
#include <map>
#include <android/asset_manager.h>

//#include <cvrUtil/AndroidStdio.h>

#include <stack>
#include <iosfwd>
#include <GLES3/gl32.h>

#include <android/log.h>
#define TAG "GLES-TEMPLATE"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__)

class assetLoader{
private:
    static assetLoader* _myPtr;
    AAssetManager * const _asset_manager;

    bool GL_checkGlError(const char* funcName);
    GLuint GL_createShader(GLenum shaderType, const char *pSource);
//    GLuint GL_createProgram(const char* vtxSrc, const char* fragSrc);

    bool LoadTextFileFromAssetManager(const char* file_name, std::string* out_file_text_string);

public:
    static assetLoader * instance();
    assetLoader(AAssetManager * assetManager);
    GLuint GL_createProgram(const char* vtxSrc, const char* fragSrc);
    GLuint createGLShaderProgramFromFile(const char* vert_file, const char *_frag_file);

};



#endif
