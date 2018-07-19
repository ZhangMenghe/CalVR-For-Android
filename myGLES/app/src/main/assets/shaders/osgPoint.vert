uniform mat4 uMVP;
attribute vec4 vPosition;
void main(){
    gl_PointSize = 15.0;
    gl_Position = vec4(vPosition.xyz, 1.0);
}
