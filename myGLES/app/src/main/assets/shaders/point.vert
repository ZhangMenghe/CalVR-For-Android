attribute vec4 vPosition;
uniform mat4 uMVP;
uniform float uPointSize;
void main(){
    gl_PointSize = uPointSize;
    gl_Position = uMVP * vec4(vPosition.xyz, 1.0);
}