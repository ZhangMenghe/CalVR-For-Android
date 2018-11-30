uniform vec3 lightPosition;
varying vec3 normal, eyeVec, lightDir;

// For the Texture
varying vec2 vTexCoord;

void main() {

    // light direction
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = normalize(vec3(lightPosition - vertextInEye.xyz));

    // normal and position
    frag_normal = normalize(vec3(gl_NormalMatrix * gl_Normal));
    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;

    // texture
    vTexCoord = gl_MultiTexCoord0.xy;
}