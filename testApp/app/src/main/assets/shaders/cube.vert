#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uModelMat, uViewMat, uProjMat;

void main(){
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(aPos, 1.0f);
}
