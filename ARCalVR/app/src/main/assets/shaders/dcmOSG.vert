#version 300 es
precision mediump float;
in vec4 osg_Vertex;

uniform mat4 uModelMat, uViewMat, uProjMat;
uniform vec3 uVolume_size;

out vec3 vTexCoord;

void main(){
    vec3 step_size = vec3(1.0/uVolume_size.x, 1.0/uVolume_size.y, 1.0/uVolume_size.z);
    vec3 tex_coord = osg_Vertex.xyz + 0.5;
    vTexCoord = clamp(tex_coord, step_size, 1.0 - step_size);
    gl_Position =  uProjMat * uViewMat * uModelMat* osg_Vertex;
}