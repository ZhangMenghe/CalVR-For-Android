uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform float shininess;
uniform sampler2D uSampler;

varying vec3 normal, eyeVec, lightDir;
varying vec2 vTexCoord;

void main(){
    vec4 finalColor = texture2D(uSampler, vTexCoord);
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