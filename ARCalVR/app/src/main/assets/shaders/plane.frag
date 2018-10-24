//uniform sampler2D uTexture;
uniform vec3 uColor;
//varying vec2 v_textureCoords;
varying float v_alpha;

void main() {
//  float r = texture2D(uTexture, v_textureCoords).r;
  float r = 1.0;
  gl_FragColor = vec4(uColor.xyz, r * v_alpha);
}