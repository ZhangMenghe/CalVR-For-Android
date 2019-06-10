#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform sampler3D uSampler_tex;

void main(){

	float intensity = texture(uSampler_tex, vTexcoord).r;
	if(gl_FrontFacing)
		gl_FragColor = vec4(intensity);//, intensity, intensity, 0.8);
	else
		gl_FragColor = vec4(vec3(intensity), 1.0 - intensity);
}
