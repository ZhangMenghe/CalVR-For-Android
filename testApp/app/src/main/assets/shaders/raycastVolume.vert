#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aTexCoord;

out vec3 frag_position; // in object space
out vec3 tex_coord;
uniform mat4 uModelMat, uViewMat, uProjMat;

void main(void)
{
    tex_coord =  aTexCoord;
    frag_position = vec3(uModelMat * vec4(aPosition, 1.0));
    gl_Position = uProjMat * uViewMat  * vec4(frag_position, 1.0);
}
