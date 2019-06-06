#version 300 es

layout (location = 0) in vec3 aPos;

out vec3 ray_start;
out vec3 ray_dir;
uniform mat4 uModelMat, uViewMat, uProjMat;

void main(){
    gl_PointSize = 10.0;
    ray_start = aPos + 0.5;

    mat4 modelViewMatrix = uViewMat * uModelMat;
    ray_dir = vec3(inverse(modelViewMatrix) * vec4(0.0,0.0,-1.0,0.0));

    gl_Position = uProjMat * uViewMat * uModelMat * vec4(aPos, 1.0f);
}
