#version 300 es
out vec4 FragColor;
uniform vec3 uBaseColor;

void main(){
	FragColor = vec4(uBaseColor, 1.0);
}