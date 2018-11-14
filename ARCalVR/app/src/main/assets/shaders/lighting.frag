uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform float shininess;
uniform vec4 uBaseColor;
varying vec3 normal, eyeVec, lightDir;

void main(){
//    vec4 finalColor = gl_FrontLightModelProduct.sceneColor;
    vec4 finalColor = uBaseColor;
    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    float lambert = dot(N,L);
    if (lambert > 0.0)
    {
        finalColor += lightDiffuse * lambert;
        vec3 E = normalize(eyeVec);
        vec3 R = reflect(-L, N);
        float specular = pow(max(dot(R, E), 0.0), shininess);
        finalColor += lightSpecular * specular;
    }
    gl_FragColor = finalColor;
}