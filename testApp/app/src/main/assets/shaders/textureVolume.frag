#version 300 es
precision mediump float;

out vec4 gl_FragColor;
in vec3 vTexcoord;
in vec2 vpos;
uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;

uniform bool u_use_color_transfer;
void main(){
	float intensity = texture(uSampler_tex, vTexcoord).r;
	vec3 sampled_color;
	if(u_use_color_transfer == true)
		sampled_color = texture(uSampler_trans, vec2(intensity, 1.0)).rgb;
	else
		sampled_color = vec3(intensity);

	if(gl_FrontFacing)
	gl_FragColor = vec4(sampled_color, intensity);//, intensity, intensity, 0.8);
	else
	gl_FragColor = vec4(sampled_color, 1.0 - intensity);
}

//if(all(greaterThanEqual(vpos.xy, vec2(0.5))) || all(lessThanEqual(vpos, vec2(-0.5))))
//gl_FragColor = vec4(vec3(intensity), 1.0);
//else{
//	if(gl_FrontFacing)
//	gl_FragColor = vec4(intensity);//, intensity, intensity, 0.8);
//	else
//	gl_FragColor = vec4(vec3(intensity), 1.0 - intensity);
//}