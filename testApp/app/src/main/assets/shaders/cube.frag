#version 300 es
out vec4 FragColor;

in vec3 ray_start;
in vec3 ray_dir;

struct cut_plane{
	vec3 point; vec3 normal;
};
struct cut_sphere{
	vec3 center; float radius;
};
uniform cut_plane cp;
uniform cut_sphere cs;

vec2 RayCube(vec3 orig, vec3 dir) {
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
float RayPlane(vec3 ro, vec3 rd, vec3 planep, vec3 planen) {
	float d = dot(planen, rd);
	float t = dot(planep - ro, planen);
	if(d > 1e-5)
	return t/d;
	else if (t > .0)
	return 1e5;//front, no effect
	return -1e5;//behind, discard
}
vec2 RaySphere(in vec3 ro, in vec3 rd, in vec3 pos, in float radius){
	float a = dot(rd, rd);
	float b = 2.0 * dot(rd, ro - pos);
	float c = dot(pos, pos) + dot(ro, ro) - 2.0 * dot(pos, ro) - radius*radius;
	float discriminant = b*b - 4.0*a*c;
	if(discriminant < 1e-5)//no intersect, ignore
		return vec2(1e5);
	discriminant = sqrt(discriminant);
	float t0 = -b - discriminant;
	float t1 = -b + discriminant;
	if(t0 * t1 <= .0) return vec2(-1e5);//discard
	return vec2(t0, t1) / (2.0 * a);
}
void main(){
	//UNIT CUBE
	vec2 intersect = RayCube(ray_start, ray_dir);
	intersect.x = max(.0, intersect.x);intersect.y = min(intersect.y, 1.0);
	intersect.y = min(intersect.y, RayPlane(ray_start, ray_dir, cp.point, cp.normal));
	if(intersect.y < intersect.x) discard;
	intersect.y = min(intersect.y, RaySphere(ray_start, vec3(cs.center - ray_start), cs.center, cs.radius).x);
	if(intersect.y < intersect.x) discard;

	FragColor = vec4(0.1, 0.5, 0.8, 1.0);
}
