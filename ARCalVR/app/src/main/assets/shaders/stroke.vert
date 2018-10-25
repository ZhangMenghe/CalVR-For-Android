attribute vec4 vPosition;
attribute vec2 vOffset;
uniform mat4 uProj;
uniform mat4 uView;
uniform float uPointSize;

void main(){
    gl_PointSize = uPointSize;
    gl_Position = uProj *uView* vec4(vPosition.xyz, 1.0) + vec4(vOffset.x, vOffset.y, .0, 1.0);
}