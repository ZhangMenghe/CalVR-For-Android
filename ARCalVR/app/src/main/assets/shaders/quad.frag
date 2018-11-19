varying vec2 v_TexCoord;
uniform sampler2D uSampler;

void main() {
    gl_FragColor = texture2D(uSampler, v_TexCoord);
}