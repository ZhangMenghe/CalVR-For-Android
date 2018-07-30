attribute vec4 vPosition;
uniform mat4 uProjMat;
uniform mat4 uViewMat;
uniform float uPointSize;
void main(){
    gl_PointSize = uPointSize;
    gl_Position = uProjMat * uViewMat * vec4(vPosition.xyz, 1.0);
}