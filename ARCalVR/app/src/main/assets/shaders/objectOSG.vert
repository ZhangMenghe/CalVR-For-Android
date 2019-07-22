varying vec2 vTexCoord;

void main(){
    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}