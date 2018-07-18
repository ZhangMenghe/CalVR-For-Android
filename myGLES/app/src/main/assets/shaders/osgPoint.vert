uniform mat4 uMVP;
//uniform float uPointSize;
void main(){
    gl_PointSize = 50;
    gl_Position = vec4(gl_Vertex.xyz, 1.0);
}
