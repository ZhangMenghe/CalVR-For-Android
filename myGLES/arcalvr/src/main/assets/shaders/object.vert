uniform mat4 uMV;
uniform mat4 uMVP;

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTexcoord;

varying vec3 v_ViewPosition;
varying vec3 v_ViewNormal;
varying vec2 v_TexCoord;

void main() {
    v_ViewPosition = (uMV * vPosition).xyz;
    v_ViewNormal = normalize((uMV * vec4(vNormal, 0.0)).xyz);
    v_TexCoord = vTexcoord;
    gl_Position = uMVP * vPosition;
}