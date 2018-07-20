attribute vec3 vNormal;

uniform mat4 uMVP;
uniform mat4 uMV;

varying vec2 v_texCoord;
varying vec3 v_viewNormal;
varying vec3 v_viewPosition;

void main() {
    v_viewPosition = (uMV * gl_Vertex).xyz;
    v_viewNormal = normalize((uMV * vec4(vNormal, 1.0)).xyz);
	v_texCoord   = gl_MultiTexCoord0.xy;
	gl_Position = uMVP * gl_Vertex;
}
