precision mediump float;
uniform mat4 uMVP;
uniform float uPointSize;
attribute vec4 vPosition;
void main(){
    gl_PointSize = uPointSize;
    gl_Position =  uMVP * vec4(vPosition.xyz, 1.0);
}
