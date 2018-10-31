uniform mat4 uModel, uView, uProj;

void main(){
    gl_Position =  uProj * uView *uModel*
                    vec4(gl_Vertex.x, gl_Vertex.z, -gl_Vertex.y, gl_Vertex.w);
}