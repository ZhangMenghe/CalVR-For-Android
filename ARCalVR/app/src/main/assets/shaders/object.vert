uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;
varying vec2 texCoord;
uniform mat4 uarMVP;
uniform mat4 uModel;
uniform float uScale;
void main(){
    texCoord = gl_Vertex.xy;
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);
    normal = gl_NormalMatrix * gl_Normal;
    gl_Position =  uarMVP *uModel* vec4(gl_Vertex.x* uScale, gl_Vertex.z* uScale, -gl_Vertex.y*uScale, gl_Vertex.w);
}