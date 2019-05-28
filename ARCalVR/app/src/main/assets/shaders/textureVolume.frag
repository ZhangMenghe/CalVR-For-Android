#version 300 es

out vec4 gl_FragColor;
in vec3 vTexcoord;

uniform sampler3D uSampler_tex;

void main(){
	//float intensity = scale * texture(uSampler, vTexcoord).r + offset;
	//gl_FragColor=vec4(intensity);
	float intensity = texture(uSampler_tex, vTexcoord).r;
	gl_FragColor = vec4(intensity, intensity, intensity, 1.0);
}
