attribute vec4 vPosition;
uniform mat4 uMVP;
void main(){
    gl_PointSize = 5.0;
    gl_Position = uMVP * vec4(vPosition.xyz, 1.0);
}