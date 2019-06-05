#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform sampler3D uSampler_tex;

void main(){

	float intensity = texture(uSampler_tex, vTexcoord).r;
	gl_FragColor = vec4(intensity, intensity, intensity, 1.0);
}
