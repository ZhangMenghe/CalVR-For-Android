#version 330 core

in vec3 frag_position; // in object space
in vec3 tex_coord;

uniform sampler3D uSampler_tex;
uniform sampler1D uSampler_trans;
uniform vec3 uEyePos;      // camera position in object space
uniform float sample_step_inverse;      // step used to advance the sampling ray
uniform float val_threshold;

void main(void)
{
  float sample_step = 1.0/sample_step_inverse;
  const float brightness = 50.0;
  // NOTE: ray direction goes from frag_position to uEyePos, i.e. back to front
  vec3 ray_dir = normalize(uEyePos - frag_position);
  vec3 ray_pos = tex_coord; // the current ray position
  vec3 pos111 = vec3(1.0, 1.0, 1.0);
  vec3 pos000 = vec3(0.0, 0.0, 0.0);

  vec4 frag_color = vec4(0,0,0,0);
  vec4 color;
  do
  {
    // note:
    // - ray_dir * sample_step can be precomputed
    // - we assume the volume has a cube-like shape

    ray_pos += ray_dir * sample_step;

    // break out if ray reached the end of the cube.
    if (any(greaterThan(ray_pos,pos111)))
      break;

    if (any(lessThan(ray_pos,pos000)))
      break;

    float density = texture(uSampler_tex, ray_pos).r;
    //frag_color = vec4(density);
     density += val_threshold - 0.5;
     density = density*density*density;
    //
    color.rgb = texture(uSampler_trans, density).rgb;
     color.a   = density * sample_step * brightness;
     //frag_color = color;
    frag_color.rgb = frag_color.rgb * (1.0 - color.a) + color.rgb * color.a;
  }
  while(true);

  if(frag_color.r == 0)
    discard;
  else
    gl_FragColor = vec4(frag_color.rgb,1.0);

}
