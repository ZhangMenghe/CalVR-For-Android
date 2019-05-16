#version 300 es

layout (location = 0) in vec3 aPosition;

uniform mat4 uModelMat, uViewMat, uProjMat;

void main(void)
{
    vec3 frag_position = vec3(uModelMat * vec4(aPosition, 1.0));
    gl_Position = uProjMat * uViewMat  * vec4(frag_position, 1.0);
}
