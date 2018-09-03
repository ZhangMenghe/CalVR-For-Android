uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;
uniform mat4 uProjMat;
uniform mat4 uViewMat;
void main(){
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);
    normal = gl_NormalMatrix * gl_Normal;
    gl_Position = uProjMat * uViewMat * gl_Vertex;
}