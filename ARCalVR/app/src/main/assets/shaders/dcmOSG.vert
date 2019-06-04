#version 300 es
precision mediump float;
in vec4 osg_Vertex;

uniform mat4 uModelMat, uViewMat, uProjMat;
uniform vec3 uVolume_size;


out vec3 frag_position;
out vec3 vTexCoord;
out vec3 ray_dir;
out mat3 NormalMatrix;



void main(){
    vec3 step_size = vec3(1.0/uVolume_size.x, 1.0/uVolume_size.y, 1.0/uVolume_size.z);
    vec3 tex_coord = osg_Vertex.xyz + 0.5;
    vTexCoord = clamp(tex_coord, step_size, 1.0 - step_size);

    frag_position = vec3(uModelMat * osg_Vertex);

    mat4 modelViewMatrix = uViewMat * uModelMat;
    ray_dir = vec3(inverse(modelViewMatrix) * vec4(0.0,0.0,-1.0,0.0));
    NormalMatrix = mat3(modelViewMatrix[0].xyz,modelViewMatrix[1].xyz,modelViewMatrix[2].xyz);

    gl_Position =  uProjMat * uViewMat * vec4(frag_position,1.0) ;
}