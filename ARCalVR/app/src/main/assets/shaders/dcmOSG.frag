#version 300 es
precision mediump float;

uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;
uniform vec3 uVolume_size;
uniform vec3 uEyePos;
//lighting
struct LightInfo{
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};
uniform LightInfo Light;
uniform float shiness;
uniform bool u_use_ligting;
uniform bool u_use_transfercolor;

//params
uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;
uniform float OpacityThreshold;

in vec3 frag_position;
in vec3 vTexCoord;
in vec3 ray_dir;
in mat3 NormalMatrix;
out vec4 gl_FragColor;

vec3 pos111 = vec3(1.0, 1.0, 1.0);
vec3 pos000 = vec3(0.0, 0.0, 0.0);

vec3 PlanePoint = vec3(.0);
vec3 PlaneNormal = vec3(.0, .0, 1.0);

void main(){
    float sample_step = 1.0/sample_step_inverse;
    vec3 ray_pos = vTexCoord; // the current ray position
    vec4 frag_color = vec4(0), color;
    float density, max_density = -1.0;
    vec3 best_ray_pos = ray_pos;
    float acc_alpha = 0.0;
    do{
        ray_pos += ray_dir * sample_step;
        if (any(greaterThan(ray_pos,pos111)))
        break;

        if (any(lessThan(ray_pos,pos000)))
        break;
        density = texture(uSampler_tex, ray_pos).r;
        float density_tmp = density+val_threshold - 0.5;
        density_tmp = density_tmp * density_tmp * density_tmp;
        acc_alpha += density_tmp*(1.0 - acc_alpha);
        max_density = max(max_density, density);
        if(max_density == density) best_ray_pos = ray_pos;
    }while(true);
    if(max_density > -1.0){
        density = max_density;
        density += val_threshold - 0.5;
        density = density * density * density;
        if(u_use_transfercolor)
            color.rgb = texture(uSampler_trans, vec2(density, 1.0)).rgb;
        else
            color.rgb = vec3(density);

        if(density < OpacityThreshold)
            color.a = .0;
        else
            color.a   = density * sample_step * brightness;
        frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
    }
    acc_alpha = clamp(acc_alpha, 0.0, 1.0);
    if(frag_color.r == 0.0)
        discard;
    else
        gl_FragColor = vec4(frag_color.rgb, color.a);
}
vec2 RayCube(vec3 ro, vec3 rd, vec3 extents) {
    vec3 tMin = (-extents - ro) / rd;
    vec3 tMax = (extents - ro) / rd;
    vec3 t1 = min(tMin, tMax);
    vec3 t2 = max(tMin, tMax);
    float tNear = max(max(t1.x, t1.y), t1.z);
    float tFar = min(min(t2.x, t2.y), t2.z);
    return vec2(tNear, tFar);
}
float RayPlane(vec3 ro, vec3 rd, vec3 planep, vec3 planen) {
    float d = dot(planen, rd);
    float t = dot(planep - ro, planen);
    if(d >  1e-5)
        return t/d;
    else if (t > .0)
        return 1e5;
    return -1e5;
}

void main_old(){
    vec2 intersect = RayCube(vTexCoord-0.5, ray_dir, vec3(.5));
    intersect.x = max(.0, intersect.x);

    intersect.x = max(intersect.x, RayPlane(vTexCoord-0.5, ray_dir, PlanePoint, PlaneNormal));
    intersect.y = min(intersect.y, 1.0);


    float sample_step = 1.0/sample_step_inverse;
    vec3 ray_pos = vTexCoord; // the current ray position
    vec4 frag_color = vec4(0), color;
    float density, max_density = -1.0;
    vec3 best_ray_pos = ray_pos;
    float acc_alpha = 0.0;

    for (float t = intersect.x; t < intersect.y; t+=sample_step) {
        ray_pos = vTexCoord + ray_dir * t;
        density = texture(uSampler_tex, ray_pos).r;
        float density_tmp = density+val_threshold - 0.5;
        density_tmp = density_tmp * density_tmp * density_tmp;
        acc_alpha += density_tmp*(1.0 - acc_alpha);
        max_density = max(max_density, density);
        if(max_density == density) best_ray_pos = ray_pos;
    }
    if(max_density > -1.0){
        density = max_density;
        density += val_threshold - 0.5;
        density = density * density * density;
        color.rgb = vec3(density);
        if(density < OpacityThreshold)
        color.a = .0;
        else
        color.a   = density * sample_step * brightness;
        frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
    }
    acc_alpha = clamp(acc_alpha, 0.0, 1.0);
    if(frag_color.r == 0.0)
    discard;
    else
    gl_FragColor = vec4(frag_color.rgb, color.a);
//    gl_FragColor = vec4(vTexCoord, 1.0);

//    float density = texture(uSampler_tex, vTexCoord).r;
//    gl_FragColor = vec4(vec3(density), 1.0);

}