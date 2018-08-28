attribute vec4 vPosition;
attribute vec2 vTexcoord;
uniform mat4 uProj;
varying vec2 texcoord;

void main(){
    gl_Position = uProj * vPosition;
    texcoord = vTexcoord;
}