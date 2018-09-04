uniform mat4 uProjMat;
uniform mat4 uViewMat;
void main(){
    gl_Position = uProjMat * uViewMat * gl_Vertex;
}