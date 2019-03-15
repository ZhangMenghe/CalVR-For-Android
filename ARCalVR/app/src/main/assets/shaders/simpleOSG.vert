uniform vec3 lightPosition;

//uniform mat4 uView, uProj, uModel, uModelView;

varying vec3 normal, eyeVec, lightDir;

void main() {

    // light direction
    // view * parent * child * vertex
//    vec4 vertexInEye = uModelView * gl_Vertex;
    vec4 vertextInEye = gl_ModelViewMatrix * gl_Vertex;     // cannot multiply by ParentModelMtx
//    vec4 vertextInEye = uView * uModel * gl_Vertex;
    eyeVec = -vertextInEye.xyz;
    lightDir = normalize(vec3(lightPosition - vertextInEye.xyz));

//    mat4 normalMtx = transpose(inverse(gl_ModelViewMatrix));  // DON"T DO THIS breaks
//    normal = normalize(vec3(normalMtx * gl_Normal));

    // normal and position
    normal = normalize(vec3(gl_NormalMatrix * gl_Normal));  // cannot multiply by ParentModelMtx
//    gl_Position = uProj * gl_ModelViewMatrix * gl_Vertex;
    gl_Position =  gl_ModelViewProjectionMatrix * gl_Vertex;
}