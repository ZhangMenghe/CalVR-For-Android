varying vec2 vTexcoord;
varying vec3 vNormal, vFragPos;
void main(){
    vTexcoord = gl_MultiTexCoord0.xy;
    vFragPos = vec3(gl_ModelViewMatrix * gl_Vertex);
    vNormal = gl_NormalMatrix * gl_Normal;
    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;
}