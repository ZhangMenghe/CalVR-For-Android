#version 300 es
precision mediump float;
uniform sampler2D uSample_trans;
out vec4 gl_FragColor;
in vec2 vTexcoord;
void main(){
    gl_FragColor = vec4(texture(uSample_trans, vTexcoord).rgb, 1.0);
}