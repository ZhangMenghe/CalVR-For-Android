//
// Created by menghe on 6/28/2018.
//
#include <jni.h>
#include <string>
#include <GLES2/gl2.h>
#include <android/log.h>


using namespace std;

bool setupGraphics(int width, int height);
void renderFrame();


extern "C"{
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_MainActivity_step(JNIEnv*env, jobject obj);
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_MainActivity_init(JNIEnv*env, jobject obj, jint width, jint height);
};

JNIEXPORT void JNICALL Java_com_example_menghe_mygles_MainActivity_step(JNIEnv*env, jobject obj){
    renderFrame();
}
JNIEXPORT void JNICALL Java_com_example_menghe_mygles_MainActivity_init(JNIEnv*env, jobject obj, jint width, jint height){
    setupGraphics(width, height);
}

#define  LOG_TAG    "libgl2jni"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error
                                                    = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

GLuint gProgram;
GLuint gvPositionHandle;

auto gVertexShader =
        "attribute vec4 vPosition;\n"
                "void main() {\n"
                "  gl_Position = vPosition;\n"
                "}\n";

auto gFragmentShader =
        "precision mediump float;\n"
                "void main() {\n"
                "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
                "}\n";

static void checkGlError(char* op);
GLuint loadShader(GLenum shaderType, const char* pSource) {
    GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char* buf = (char*) malloc(infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}
GLuint createProgram(const char* pVertexSource, const char* pFragmentSource) {
    GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);
    if (!vertexShader) {
        return 0;
    }

    GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);
    if (!pixelShader) {
        return 0;
    }

    GLuint program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShader);

        glAttachShader(program, pixelShader);

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = (char*) malloc(bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }
    }
    return program;
}

bool setupGraphics(int width, int height){
    gProgram = createProgram(gVertexShader, gFragmentShader);
    if (!gProgram) {
        return false;
    }
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");


    glViewport(0, 0, width, height);



    return true;
}
const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
                                      0.5f, -0.5f };

void renderFrame() {
    static float grey = 0.5f;
//    grey += 0.01f;
//    if (grey > 1.0f) {
//        grey = 0.0f;
//    }
    glClearColor(grey, grey, grey, 1.0f);

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glUseProgram(gProgram);

//    GLuint mvpHandler = glGetUniformLocation(gProgram, "uMVPMatrix");
//    glUniformMatrix4fv(mvpHandler, 1, false, mvpMat, 0)

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);

    glEnableVertexAttribArray(gvPositionHandle);

    glDrawArrays(GL_TRIANGLES, 0, 3);

}
