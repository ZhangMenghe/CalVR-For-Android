uniform vec3 uLightPos;

varying vec3 vNormal, vEyeVec, vLightDir;
varying vec2 vTexCoord;

void main(){
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    vEyeVec = -vertextInEye.xyz;
    vLightDir = vec3(uLightPos - vertextInEye.xyz);
    vNormal = gl_NormalMatrix * gl_Normal;

    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}