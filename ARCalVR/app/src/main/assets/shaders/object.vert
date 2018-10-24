uniform mat4 uModel, uView, uProj;
uniform vec3 lightPosition;
uniform float uScale;

varying vec3 normal, eyeVec, lightDir;
varying vec2 vTexCoord;

void main(){
    vTexCoord = vec2(0.5,0.5);//gl_Vertex.xz;
    vec4 aPosition = vec4(gl_Vertex.x* uScale, gl_Vertex.z* uScale, -gl_Vertex.y*uScale, gl_Vertex.w);

    mat4 modelViewMat = uView * uModel;
    vec4 vertextInEye = modelViewMat * aPosition;
    eyeVec = -vertextInEye.xyz;
    lightDir = vec3(lightPosition - vertextInEye.xyz);

    normal = gl_NormalMatrix * gl_Normal;
    gl_Position =  uProj * uView *uModel* aPosition;
}