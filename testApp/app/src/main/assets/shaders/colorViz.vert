#version 300 es
precision mediump float;
layout (location = 0) in vec2 aPosition;
layout (location = 1) in vec2 aTexcoord;
uniform vec2 uOffset;
uniform vec2 uScale;
out vec2 vTexcoord;
void main(){
    vTexcoord = aTexcoord;
    gl_Position = vec4(aPosition * uScale + uOffset, .0, 1.0);
}