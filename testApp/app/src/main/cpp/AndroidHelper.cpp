#include "AndroidHelper.h"
assetLoader* assetLoader::_myPtr = nullptr;

assetLoader::assetLoader(AAssetManager * const assetManager):
_asset_manager(assetManager){
    _myPtr = this;
}
assetLoader* assetLoader::instance() {
    return _myPtr;
}

bool assetLoader::GL_checkGlError(const char *funcName) {
    GLint err = glGetError();
    if (err != GL_NO_ERROR) {
        LOGE("GL error after %s(): 0x%08x\n", funcName, err);
        return true;
    }
    return false;
}
GLuint assetLoader::GL_createShader(GLenum shaderType, const char *pSource){
    GLuint shader = glCreateShader(shaderType);
    if (!shader) {
        GL_checkGlError("glCreateShader");
        return 0;
    }
    glShaderSource(shader, 1, &pSource, NULL);

    GLint compiled = GL_FALSE;
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLogLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen > 0) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetShaderInfoLog(shader, infoLogLen, NULL, infoLog);
                LOGE("Could not compile %s shader:\n%s\n",
                      shaderType == GL_VERTEX_SHADER ? "vertex" : "fragment",
                      infoLog);
                free(infoLog);
            }
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}
GLuint assetLoader::GL_createProgram(const char* vtxSrc, const char* fragSrc){
    GLuint vtxShader = 0;
    GLuint fragShader = 0;
    GLuint program = 0;
    GLint linked = GL_FALSE;

    vtxShader = GL_createShader(GL_VERTEX_SHADER, vtxSrc);
    if (!vtxShader)
        goto exit;

    fragShader = GL_createShader(GL_FRAGMENT_SHADER, fragSrc);
    if (!fragShader)
        goto exit;

    program = glCreateProgram();
    if (!program) {
        GL_checkGlError("glCreateProgram");
        goto exit;
    }
    glAttachShader(program, vtxShader);
    glAttachShader(program, fragShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked) {
        LOGE("Could not link program");
        GLint infoLogLen = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLen);
        if (infoLogLen) {
            GLchar* infoLog = (GLchar*)malloc(infoLogLen);
            if (infoLog) {
                glGetProgramInfoLog(program, infoLogLen, NULL, infoLog);
                LOGE("Could not link program:\n%s\n", infoLog);
                free(infoLog);
            }
        }
        glDeleteProgram(program);
        program = 0;
    }

    exit:
    glDeleteShader(vtxShader);
    glDeleteShader(fragShader);
    return program;
}

bool assetLoader::LoadTextFileFromAssetManager(const char* file_name,std::string* out_file_text_string) {
    AAsset* asset =
            AAssetManager_open(_asset_manager, file_name, AASSET_MODE_STREAMING);
    if (asset == nullptr) {
        LOGE("Error opening asset %s", file_name);
        return false;
    }

    off_t file_size = AAsset_getLength(asset);
    out_file_text_string->resize(file_size);
    int ret = AAsset_read(asset, &out_file_text_string->front(), file_size);

    if (ret <= 0) {
        LOGE("Failed to open file: %s", file_name);
        AAsset_close(asset);
        return false;
    }

    AAsset_close(asset);
    return true;
}
GLuint assetLoader::createGLShaderProgramFromFile(const char* vert_file, const char *_frag_file){
    std::string VertexShaderContent;
    if (!LoadTextFileFromAssetManager(vert_file, &VertexShaderContent)) {
        LOGE("Failed to load file: %s", vert_file);
        return 0;
    }

    std::string FragmentShaderContent;
    if (!LoadTextFileFromAssetManager(_frag_file, &FragmentShaderContent)) {
        LOGE("Failed to load file: %s", _frag_file);
        return 0;
    }

    return GL_createProgram(VertexShaderContent.c_str(), FragmentShaderContent.c_str());
}

