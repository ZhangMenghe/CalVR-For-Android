attribute vec4 vPosition;
uniform mat4 uarMVP;
uniform float uPointSize;
void main(){
    gl_PointSize = uPointSize;
    // uProjMat * uViewMat *
    gl_Position = uarMVP * vec4(vPosition.xyz, 1.0);
}