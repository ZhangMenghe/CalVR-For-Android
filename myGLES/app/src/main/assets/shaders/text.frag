uniform sampler2D uTexture;
//uniform vec3 texColor;

varying vec2 texcoord;
void main(){
   float alpha = texture2D(uTexture, texcoord).a;
   gl_FragColor = vec4(0.0, 1.0, 0.0, alpha);
}