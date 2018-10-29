uniform mat4 uModel, uView, uProj;
uniform vec3 lightPosition;

varying vec3 normal, eyeVec, lightDir;
varying vec2 vTexCoord;

void main(){
    mat4 modelViewMat = uView * uModel;
    vec4 vertextInEye = modelViewMat * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);

    normal = gl_NormalMatrix * gl_Normal;
    gl_Position =  uProj * uView *uModel* gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}