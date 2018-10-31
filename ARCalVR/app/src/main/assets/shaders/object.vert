uniform mat4 uModelView, uProj;
uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;
varying vec2 vTexCoord;

void main(){
    vec4 vertextInEye = uModelView * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);
    normal = gl_NormalMatrix  *gl_Normal;//mat3(transpose(inverse(uModelView)))  * gl_Normal;
    gl_Position =  uProj * uModelView* gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}