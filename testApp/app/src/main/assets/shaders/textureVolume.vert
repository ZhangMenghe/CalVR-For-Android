#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

uniform mat4 uModelMat, uViewMat, uProjMat;
out vec3 vTexcoord;
uniform vec3 volumesize;
void main(){
    vTexcoord = aTexCoord;
    vTexcoord = clamp(vTexcoord, 1.0 / volumesize, 1.0 - 1.0/volumesize);
    gl_Position = uProjMat * uViewMat  * uModelMat * vec4(aPosition, 1.0);
}
