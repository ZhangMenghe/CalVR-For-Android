varying vec2 v_texCoord;
void main() {
//gl_ModelViewProjectionMatrix *
	gl_Position = gl_Vertex;
	v_texCoord   = gl_MultiTexCoord0.xy;
}
