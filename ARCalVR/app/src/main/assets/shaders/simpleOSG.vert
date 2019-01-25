uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;

void main() {

    // light direction
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);

    // normal and position
    normal = vec3(gl_NormalMatrix * gl_Normal);
    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;
}