uniform mat4 uMVP;
uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;

void main(){
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);
    normal = gl_NormalMatrix * gl_Normal;

    gl_Position = gl_ModelViewProjectionMatrix * uMVP * gl_Vertex;
}