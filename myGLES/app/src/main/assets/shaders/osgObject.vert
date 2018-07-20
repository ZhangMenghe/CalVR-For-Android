uniform mat4 uMVP;
uniform mat4 uMV;

varying vec2 v_texCoord;
//varying vec3 v_viewNormal;
//varying vec3 v_viewPosition;

void main() {
//    v_viewPosition = (uMV * gl_Vertex).xyz;
//    v_viewNormal = normalize((uMV * gl_Normal).xyz);
	v_texCoord   = gl_MultiTexCoord0.xy;
	gl_Position = uMVP * gl_Vertex;
}
