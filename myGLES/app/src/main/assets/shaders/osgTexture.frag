#extension GL_OES_EGL_image_external : require
#ifdef GL_FRAGMENT_PRECISION_HIGH
precision highp float;
#else
precision mediump float;
#endif

varying  mediump vec2 v_texCoord;
uniform samplerExternalOES uTexture;
void main() {
	gl_FragColor = texture2D(uTexture, v_texCoord);
}