uniform vec3 ulightPosition;
uniform mat4 uMVP;
uniform mat4 uMV;

varying vec2 v_texCoord;
//varying vec3 v_viewNormal;
//varying vec3 v_viewPosition;

varying vec3 normal, eyeVec, lightDir;

void main() {
    v_texCoord = gl_MultiTexCoord0.xy;
    vec4 vertextInEye = uMV * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(ulightPosition - vertextInEye.xyz);
    normal = gl_NormalMatrix * gl_Normal;
    gl_Position = uMVP * gl_Vertex;
}
