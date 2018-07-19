//uniform mat4 uMVP;
//uniform float uPointSize;
void main(){
    gl_PointSize = 10.0;
    gl_Position = gl_Vertex;
}
