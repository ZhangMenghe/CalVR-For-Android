#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform sampler3D uSampler_tex;
uniform float OpacityThreshold;

void main(){

	float intensity = texture(uSampler_tex, vTexcoord).r;
	float alpha  = intensity;
	//cut of the density
	if(alpha < OpacityThreshold)
		alpha = .0;
	else
		alpha = 1.0;
	gl_FragColor = vec4(vec3(intensity), 1.0);

}
