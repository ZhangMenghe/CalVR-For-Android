//
// Created by menghe on 6/29/2018.
//
#include <jni.h>
#include <string>
#include <GLES2/gl2.h>

#include "utils.h"
#include "drawPrimitive.h"

using namespace std;

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

const GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f,
                                      0.5f, -0.5f };

void initilization(int width, int height, AAssetManager* asset_manager){
//    gProgram = utils::CreateProgram(gVertexShader,gFragmentShader);
    gProgram = utils::CreateProgram("shaders/naive.vert", "shaders/naive.frag", asset_manager);
    gvPositionHandle = glGetAttribLocation(gProgram, "vPosition");
    glViewport(0, 0, width, height);
}
void draw(){
    static float grey = 0.5f;
    glClearColor(grey, grey, grey, 1.0f);

    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


    glUseProgram(gProgram);

//    GLuint mvpHandler = glGetUniformLocation(gProgram, "uMVPMatrix");
//    glUniformMatrix4fv(mvpHandler, 1, false, mvpMat, 0)

    glVertexAttribPointer(gvPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);

    glEnableVertexAttribArray(gvPositionHandle);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}
