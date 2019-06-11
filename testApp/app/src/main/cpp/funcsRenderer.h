#ifndef FUNCTION_RENDERER_H
#define FUNCTION_RENDERER_H
// Include the latest possible header file( GL version header )
#if __ANDROID_API__ >= 24
#include <GLES3/gl32.h>
#include <glm/vec2.hpp>

#elif __ANDROID_API__ >= 21
#include <GLES3/gl31.h>
#else
#include <GLES3/gl3.h>
#endif

enum FUNC_TYPE{
    LINEAR_FUNC = 0,
    COLOR_BAR
};
class FuncRenderer{
public:
    void InitProgram();
    void CreateFunction(FUNC_TYPE type);
    void setColorTransfer(bool color_transfer){is_color_transferd = color_transfer;}
    void setOpacityShow(bool opacity_adj){is_opacity_adj = opacity_adj;}
    void UpdateFuncPoints(FUNC_TYPE type, float* points, bool is_quad = true);
    void UpdateFuncPoints(FUNC_TYPE type, glm::vec2 p1, glm::vec2 p2, bool is_quad = true);
    void Draw(FUNC_TYPE type);
    void Draw();
protected:
    const size_t MAX_VERTICS=10;
    const GLuint indices_func_[6]= {0,1,2,
                                    0,2,3};

    GLuint VAO_FUNC, VBO_FUNC,EBO_Func;
    GLuint VAO_QUAD;
    GLuint VAO_COLOR_BAR;

    GLuint program_func, program_quad, program_color_bar;
    GLfloat *vertices_func_;
    bool is_color_transferd = false, is_opacity_adj = false;
    GLuint createQuad();
    GLuint createColorBar();
    void createLinearQuad();

    void draw_color_bar();
    void draw_opacity_func();
};
#endif