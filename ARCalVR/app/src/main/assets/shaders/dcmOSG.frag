#version 300 es
precision mediump float;

uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;
uniform vec3 uVolume_size;

//lighting
struct LightInfo{
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};
uniform LightInfo Light;
uniform float shiness;
uniform bool u_use_ligting;

//params
uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;
uniform float OpacityThreshold;

in vec3 vTexCoord;
out vec4 gl_FragColor;
void main(){
//    gl_FragColor = vec4(vTexCoord, 1.0);
    float density = texture(uSampler_tex, vTexCoord).r;
    gl_FragColor = vec4(vec3(density), 1.0);
//    gl_FragColor =texture(uSampler_trans, vec2(0.2, 1.0));
}