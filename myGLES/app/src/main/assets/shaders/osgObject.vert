uniform mat4 uMVP;
uniform mat4 uMV;

varying vec2 v_TexCoord;
varying vec3 v_ViewNormal;
varying vec3 v_ViewPosition;



void main() {
    v_TexCoord = gl_MultiTexCoord0.xy;
    v_ViewNormal = gl_NormalMatrix * gl_Normal;
    v_ViewPosition = (uMV * gl_Vertex).xyz;
    gl_Position = uMVP * gl_Vertex;
}
