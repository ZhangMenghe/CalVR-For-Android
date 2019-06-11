#include <cstring>
#include "funcsRenderer.h"
#include "AndroidHelper.h"

void FuncRenderer::InitProgram(){
    program_func = assetLoader::instance()->createGLShaderProgramFromFile("shaders/opaViz.vert", "shaders/opaViz.frag");
    program_quad = assetLoader::instance()->createGLShaderProgramFromFile("shaders/opaViz.vert", "shaders/opaViz.frag");
    if(!program_func || !program_quad)
        LOGE("===Failed to create func view shader program===");
}
void FuncRenderer::createQuad(){
    //quad
    glGenVertexArrays(1, &VAO_QUAD);
    glBindVertexArray(VAO_QUAD);

    GLuint vbo, ebo;
    float quad_vertices[]={
            0.5f, 1.0,//top-right
            -0.5f,1.0,//top-left
            -0.5f, .0f,//bottom-left
            0.5f, .0f//bottom-right
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8* sizeof(GL_FLOAT), quad_vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, indices_func_, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void FuncRenderer::createLinearQuad(){
    glGenVertexArrays(1, &VAO_FUNC);
    glBindVertexArray(VAO_FUNC);

    glGenBuffers(1, &VBO_FUNC);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_FUNC);
    glBufferData(GL_ARRAY_BUFFER, MAX_VERTICS* sizeof(GL_FLOAT), nullptr, GL_DYNAMIC_DRAW);

    vertices_func_ = new GLfloat[MAX_VERTICS * 2];
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    //todo: may not be static
    glGenBuffers(1, &EBO_Func);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_Func);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*6, indices_func_, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void FuncRenderer::CreateFunction(FUNC_TYPE type){
    if(type != LINEAR_FUNC) return;
    createLinearQuad();
    createQuad();
}
void FuncRenderer::UpdateFuncPoints(FUNC_TYPE type, glm::vec2 p1, glm::vec2 p2, bool is_quad){
    float vertices[] = {
            p2.x, p2.y,//top-right
            p1.x, p1.y,//top-left
            p1.x, .0f,//bottom-left
            0.5f, .0f//bottom-right
    };
    if(is_quad)
        memcpy(vertices_func_, vertices, 2*4*sizeof(GL_FLOAT));
    else
        memcpy(vertices_func_, vertices, 2*2*sizeof(GL_FLOAT));
}
void FuncRenderer::UpdateFuncPoints(FUNC_TYPE type, float* points, bool is_quad){
    if(is_quad)
        memcpy(vertices_func_, points, 2*4*sizeof(GL_FLOAT));
    else
        memcpy(vertices_func_, points, 2*2*sizeof(GL_FLOAT));
}
void FuncRenderer::Draw(){
    glBindBuffer(GL_ARRAY_BUFFER, VBO_FUNC);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 2*4  *sizeof(GL_FLOAT), vertices_func_);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_quad);
    glUniform2f(glGetUniformLocation(program_quad, "uOffset"), .0,-1.0f);
    glUniform2f(glGetUniformLocation(program_quad, "uScale"), 2.0f, 0.20f);
    glUniform4f(glGetUniformLocation(program_quad, "uColor"), 0.086f, 0.098f, 0.231f, 1.0f);
    glBindVertexArray(VAO_QUAD);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);


    glUseProgram(program_func);
    glUniform2f(glGetUniformLocation(program_func, "uOffset"), .0,-1.0f);
    glUniform2f(glGetUniformLocation(program_func, "uScale"), 1.8f, 0.15f);
    glUniform4f(glGetUniformLocation(program_func, "uColor"), 0.678f, 0.839f, 0.969f, 0.5f);
    glBindVertexArray(VAO_FUNC);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    glDisable(GL_BLEND);
}