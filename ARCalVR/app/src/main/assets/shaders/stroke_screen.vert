attribute vec2 vPositionScreen;
uniform float uPointSize;
void main(){
    gl_PointSize = uPointSize;
    gl_Position = vec4(vPositionScreen.x, vPositionScreen.y, .0, 1.0);
}