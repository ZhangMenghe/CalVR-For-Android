varying vec2 v_TexCoord;
uniform sampler2D uSampler;
void main() {
    gl_FragColor = texture2D(uSampler, v_TexCoord);
//    vec3 yuv;
//    yuv.x = texture2D(uSampler_y, v_TexCoord).r;
//    yuv.y = texture2D(uSampler_u, v_TexCoord).r;
//    yuv.z = texture2D(uSampler_v, v_TexCoord).r;
//    rgb = mat3( 1,       1,         1,
//             0,       -0.39465,  2.03211,
//             1.13983, -0.58060,  0) * yuv;
//    gl_FragColor = vec4(rgb, 1.0f);
}