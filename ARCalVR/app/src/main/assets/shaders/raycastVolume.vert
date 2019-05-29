#version 300 es
precision mediump float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

out vec3 frag_position; // in object space
out vec3 tex_coord;

out mat3 NormalMatrix;
out vec3 ray_dir;

uniform mat4 uModelMat, uViewMat, uProjMat;
uniform float volumex, volumey, volumez;
void main(void){
    gl_PointSize = 10.0;

    vec3 step_size = vec3(1.0 / volumex, 1.0/volumey, 1.0/volumez);
    tex_coord = clamp(aPosition + 0.5,step_size,1.0 - step_size);

    frag_position = vec3(uModelMat * vec4(aPosition, 1.0));
    mat4 modelViewMatrix = uViewMat * uModelMat;

    NormalMatrix = mat3(modelViewMatrix[0].xyz,modelViewMatrix[1].xyz,modelViewMatrix[2].xyz);
    ray_dir = vec3(inverse(modelViewMatrix) * vec4(0.0,0.0,-1.0,0.0));

    gl_Position = uProjMat * uViewMat  * vec4(frag_position, 1.0);
}