uniform mat4 uMVP;
//uniform float uPointSize;
void main(){
//    gl_PointSize = 100;
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xyz, 1.0);
}
