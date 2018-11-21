varying vec2 v_TexCoord;

void main() {
   gl_Position = vec4(gl_Vertex.x, gl_Vertex.y, .0, 1.0);
   v_TexCoord = gl_MultiTexCoord0.xy;
}
