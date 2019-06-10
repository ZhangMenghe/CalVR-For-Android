#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

uniform mat4 uModelMat, uViewMat, uProjMat;
uniform vec3 uVolumeSize;
out vec3 vTexcoord;
out vec2 vpos;
void main(){
    vec3 step_size = 1.0 / uVolumeSize;
    vTexcoord = clamp(aTexCoord, step_size, 1.0 - step_size);
    vpos = aPosition.xy;
    gl_Position = uProjMat * uViewMat  * uModelMat * vec4(aPosition, 1.0);
}
