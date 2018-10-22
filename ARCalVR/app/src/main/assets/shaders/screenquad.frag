#extension GL_OES_EGL_image_external : require
varying vec2 v_TexCoord;

uniform samplerExternalOES uTexture;
void main() {
    gl_FragColor = texture2D(uTexture, v_TexCoord);
}