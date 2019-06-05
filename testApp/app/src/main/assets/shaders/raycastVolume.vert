#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPosition;
//layout (location = 1) in vec3 aTexCoord;

out vec3 frag_position; // in object space
out vec3 tex_coord;

out mat3 NormalMatrix;
out vec3 ray_dir;

uniform mat4 uModelMat, uViewMat, uProjMat;
uniform vec3 eye_pos_world;

void main(void){
    gl_PointSize = 10.0;
    tex_coord = aPosition + 0.5;
//    tex_coord = aTexCoord;
    frag_position = vec3(uModelMat * vec4(aPosition, 1.0));
    mat4 modelViewMatrix = uViewMat * uModelMat;

    NormalMatrix = mat3(modelViewMatrix[0].xyz,modelViewMatrix[1].xyz,modelViewMatrix[2].xyz);


    ray_dir = normalize(vec3(inverse(uModelMat) * vec4(eye_pos_world,1.0)) - tex_coord);
    gl_Position = uProjMat * uViewMat  * vec4(frag_position, 1.0);
}
