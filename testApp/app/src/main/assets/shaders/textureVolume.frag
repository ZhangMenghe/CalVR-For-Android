#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;
in vec2 vpos;
uniform sampler3D uSampler_tex;

void main(){

	float intensity = texture(uSampler_tex, vTexcoord).r;
	if(gl_FrontFacing)
	gl_FragColor = vec4(intensity);//, intensity, intensity, 0.8);
	else
	gl_FragColor = vec4(vec3(intensity), 1.0 - intensity);
}

//if(all(greaterThanEqual(vpos.xy, vec2(0.5))) || all(lessThanEqual(vpos, vec2(-0.5))))
//gl_FragColor = vec4(vec3(intensity), 1.0);
//else{
//	if(gl_FrontFacing)
//	gl_FragColor = vec4(intensity);//, intensity, intensity, 0.8);
//	else
//	gl_FragColor = vec4(vec3(intensity), 1.0 - intensity);
//}