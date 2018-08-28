attribute vec3 vPosition;
varying vec2 v_textureCoords;
varying float v_alpha;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform vec3 uNormal;

void main() {
  // Vertex Z value is used as the alpha in this shader.
  v_alpha = vPosition.z;

  vec4 local_pos = vec4(vPosition.x, 0.0, vPosition.y, 1.0);
  gl_Position = uMVP * local_pos;
  vec4 world_pos = uModel * local_pos;

  // Construct two vectors that are orthogonal to the normal.
  // This arbitrary choice is not co-linear with either horizontal
  // or vertical plane normals.
  const vec3 arbitrary = vec3(1.0, 1.0, 0.0);
  vec3 vec_u = normalize(cross(uNormal, arbitrary));
  vec3 vec_v = normalize(cross(uNormal, vec_u));

  // Project vertices in world frame onto vec_u and vec_v.
  v_textureCoords = vec2(
  dot(world_pos.xyz, vec_u), dot(world_pos.xyz, vec_v));
}
