//uniform mat4 uMVP;
//uniform float uPointSize;
attribute vec4 vPosition;
void main(){
    gl_PointSize = 10.0;
    gl_Position = vPosition;
}
