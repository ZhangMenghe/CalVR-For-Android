#version 300 es
precision mediump float;
uniform vec4 uColor;
out vec4 gl_FragColor;
void main(){
    gl_FragColor = uColor;
}