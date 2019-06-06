#version 300 es
precision mediump float;

in vec3 frag_position; // in object space
in vec3 tex_coord;
in vec3 ray_dir;
in mat3 NormalMatrix;

uniform sampler3D uSampler_tex;
uniform sampler2D uSampler_trans;

uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;
uniform float brightness;
out vec4 gl_FragColor;

struct LightInfo{
    vec3 Ia;
    vec3 Id;
    vec3 Is;
};
uniform LightInfo Light;
uniform float shiness;

uniform bool u_use_color_transfer;
uniform bool u_use_ligting;
uniform bool u_use_interpolation;
uniform bool u_draw_naive;
uniform float OpacityThreshold;
uniform float volumex, volumey, volumez;
//uniform vec3 tex_limit_max, tex_limit_min;
uniform vec3 PlanePoint;// = vec3(.0, .0, 0.5);
uniform vec3 PlaneNormal;// = vec3(.0, .0, 1.0);
uniform vec3 sphere_center;
uniform float sphere_radius;

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
    return 1e5;//front, no effect
    return -1e5;//behind, discard
}
bool pointSphere(vec3 p, vec3 sc, float radius){
    return (length(p-sc) < radius);
}
float RaySphere(vec3 ro, vec3 rd, vec3 sc, float radius){

    vec3 oc = ro - sc;
    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc,oc) - radius*radius;
    float discriminant = b*b - 4.0*a*c;
    if(discriminant < 1e-5) //not intersect
        return 1e5;
    return -1e5;
    discriminant = sqrt(discriminant);
    float t1 = -b + discriminant; float t0 = -b-discriminant;
    if(t0 * t1 < 1e-5) return -1e5;//discard, point inside circle
    return t0 * 0.5;
}
// uniform vec3 u_clip_plane[6];
// uniform int u_cpoints_num;
vec3 phong_illumination_model(vec3 N,vec3 curColor,vec3 curPos){
    //compute ambient
    //                  Ka  *  Ia
    vec3 ambient = 1.5 * curColor * Light.Ia;
    //compute diffuse
    vec3 L = normalize(vec3(0,0,-1));
    //               Kd     *    Id    *  N dot L
    vec3 diffuse = 1.5 *curColor * Light.Id * max(0.0,dot(N,L));
    //compute specular
    vec3 R = reflect(L,N);
    vec3 E = normalize(-curPos);
    //                    Ks          *    Is    * (R dot E)^shiness
    vec3 specular = 1.5 *vec3(1.0,1.0,1.0) * Light.Is * pow(max(0.0,dot(R,E)),shiness);

    return (ambient + diffuse + specular);
}

vec3 getRayIntersection(vec3 ray_start, vec3 ray_dir, vec3 plane_point, vec3 plane_norm){
    float ray_plane_angle  = dot(ray_dir, plane_norm);
    if(ray_plane_angle == 0.0)
        return vec3(10.,10.,10.);
    //AX + BY + CZ + D = 0
    //D = -(PLANE_POINT * PLANE_NORM)
    //N = RAY_DIR * PLANE_NORM
    return ray_start - (( dot(plane_norm, ray_start) - dot(plane_point, plane_norm)) / ray_plane_angle) * ray_dir;
}
vec3 getPlaneNormal(vec3 a, vec3 b, vec3 c){
    return normalize(cross(b-a, c-a));
}
float getVoxel(float x, float y, float z){
    return texture(uSampler_tex, vec3(x,y,z)).r;
}
float get_trilinear_interpolation(vec3 pc, vec3 cmin, vec3 cmax){
    // return texture(uSampler_tex, p).r;
    highp float x1 = floor(pc.x * volumex) / volumex;
    highp float y1 = floor(pc.y * volumey) / volumey;
    highp float z1 = floor(pc.z * volumez) / volumez;
    vec3 p = clamp(vec3(x1, y1, z1), cmin, cmax);

    // return texture(uSampler_tex, vec3(x1, y1,z1)).r;
    vec3 p2 =clamp(vec3(x1, y1, z1) + cmin, cmin, cmax);
    float alpha = pc.x-p.x; float beta = pc.y-p.y; float gamma = pc.z-p.z;
    return  ((1.0 - alpha) * (1.0- beta) * (1.0- gamma) * getVoxel(p.x, p.y, p.z) +
                    alpha * (1.0- beta) * (1.0- gamma) * getVoxel(p2.x, p.y, p.z) +
                    (1.0- alpha) * beta * (1.0- gamma) * getVoxel(p.x, p2.y, p.z) +
                    alpha * beta * (1.0- gamma) * getVoxel(p2.x, p2.y, p.z) +
                    (1.0- alpha) * (1.0- beta) * gamma *getVoxel(p.x, p.y, p2.z) +
                    alpha * (1.0- beta) * gamma * getVoxel(p2.x, p.y, p2.z) +
                    (1.0- alpha) * beta * gamma * getVoxel(p.x, p2.y, p2.z) +
                    alpha * beta * gamma *getVoxel(p2.x, p2.y, p2.z));
}
void main_old(void){
  if(u_draw_naive){
      gl_FragColor = vec4(tex_coord, 1.0);
      return;
  }
  vec3 step_size = vec3(1.0 / volumex, 1.0/volumey, 1.0/volumez);
  float sample_step = 1.0/sample_step_inverse;
  vec3 ray_pos = tex_coord; // the current ray position
  vec3 pos111 = vec3(1.0, 1.0, 1.0);
  vec3 pos000 = vec3(0.0, 0.0, 0.0);

  vec4 frag_color = vec4(0,0,0,0);
  vec4 color;
  float density, max_density = -1.0;
  vec3 best_ray_pos = ray_pos;

  // vec3 intersect_point = getRayIntersection(ray_pos, ray_dir,
  //                                           u_clip_plane[0],
  //                                           getPlaneNormal(u_clip_plane[0], u_clip_plane[1], u_clip_plane[2]));
  // if (all(lessThan(intersect_point, pos111)))
  //   pos111 = intersect_point;
  do{
    // note:
    // - ray_dir * sample_step can be precomputed for a fixed view position/angle
    // - we assume the volume has a cube-like shape

    ray_pos += ray_dir * sample_step;

    // break out if ray reached the end of the cube.
    if (any(greaterThan(ray_pos,pos111)))
      break;

    if (any(lessThan(ray_pos,pos000)))
      break;

    density = texture(uSampler_tex, ray_pos).r;
    max_density = max(max_density, density);
    if(max_density == density){

        best_ray_pos = ray_pos;
    }

  }while(true);

    if(max_density > -1.0){
        if(u_use_interpolation)//re-sample using interpolation
            density = get_trilinear_interpolation(best_ray_pos, step_size, 1.0-step_size);
        else{
            density = max_density;
        }


        density += val_threshold - 0.5;
        density = density * density * density;

        vec3 normal = normalize(NormalMatrix*(normalize(best_ray_pos)));
        vec3 sampled_color;
        if(u_use_color_transfer == true)
            sampled_color = texture(uSampler_trans, vec2(density, 1.0)).rgb;
        else
            sampled_color = vec3(density);

        if(u_use_ligting == true)
            color.rgb = phong_illumination_model(normal, sampled_color, frag_position);
        else
            color.rgb = sampled_color;
        if(density < OpacityThreshold)
            color.a = .0;
        else
            color.a   = density * sample_step * brightness;

        frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
    }


  if(frag_color.r == 0.0)
    discard;
  else
    gl_FragColor = vec4(frag_color.rgb,1.0);
}
vec2 intersect_box(vec3 orig, vec3 dir) {
    const vec3 box_min = vec3(0);
    const vec3 box_max = vec3(1);
    vec3 inv_dir = 1.0 / dir;
    vec3 tmin_tmp = (box_min - orig) * inv_dir;
    vec3 tmax_tmp = (box_max - orig) * inv_dir;
    vec3 tmin = min(tmin_tmp, tmax_tmp);
    vec3 tmax = max(tmin_tmp, tmax_tmp);
    float t0 = max(tmin.x, max(tmin.y, tmin.z));
    float t1 = min(tmax.x, min(tmax.y, tmax.z));
    return vec2(t0, t1);
}
vec4 RaycastSampling(float s, float t){
    vec3 pos111 = vec3(1.0, 1.0, 1.0);
    vec3 pos000 = vec3(0.0, 0.0, 0.0);

    float step_delta = t - s;
    float sample_step = 1.0/sample_step_inverse;
    vec3 ray_pos = tex_coord;
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
        color.rgb = vec3(density);
        color.a   = density * sample_step * brightness;
        frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
    }
    acc_alpha = clamp(acc_alpha, 0.0, 1.0);
    if(frag_color.r == 0.0)
        return vec4(density);
    else
        return vec4(frag_color.rgb, color.a);
}
void main(){
    vec3 ray_start = tex_coord;

    vec2 intersect = intersect_box(ray_start, ray_dir);
    intersect.x = max(.0, intersect.x);intersect.y = min(intersect.y, 1.0);

    #ifdef BACK_FACE_CULLED
        intersect.x = max(intersect.x, RayPlane(ray_start, ray_dir, PlanePoint, PlaneNormal));
    #else
        intersect.y = min(intersect.y, RayPlane(ray_start, ray_dir, PlanePoint, PlaneNormal));
    #endif

    if (intersect.y < intersect.x) discard;

//    vec3 sphere_center = vec3(1.0); float sphere_radius = 0.5;
//    intersect.y = min(intersect.y, RaySphere(tex_coord, ray_dir, sphere_center, sphere_radius));
//
//    if (intersect.y < intersect.x) {
//        gl_FragColor = vec4(1.0,.0,.0,1.0);
//        return;
//        //discard;
//    }
    if(pointSphere(tex_coord, sphere_center, sphere_radius)) discard;
//    {
//                gl_FragColor = vec4(1.0,.0,.0,1.0);
//                return;
//    }
    if(u_draw_naive)
        gl_FragColor = vec4(tex_coord, 1.0);
    else
        gl_FragColor = RaycastSampling(intersect.x, intersect.y);

}


