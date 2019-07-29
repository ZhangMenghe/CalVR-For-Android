//#version 300 es

uniform bool uTextureChoice;
uniform sampler2D uSampler, uSamplerC;
uniform vec4 uColorCorrection;

uniform vec3 uEyePos;
uniform vec3 uLightPos;
uniform float uShiness;

varying vec2 vTexcoord;
varying vec3 vNormal, vFragPos;


const float kGamma = 0.4545454;
const float kInverseGamma = 2.2;
const float kMiddleGrayGamma = 0.466;

const float kSpecularStrength = 5.0;
const float kAmbientStrength = 0.5;
const vec3 kLightColor = vec3(1.0,1.0,1.0);

void main(){
//    gl_FragColor = texture2D(uSampler, vTexcoord);
    vec4 objectColor = vec4(1.0,.0,.0,1.0);
    if(uTextureChoice)
    objectColor = texture2D(uSampler, vTexcoord);
    else
    objectColor = texture2D(uSamplerC, vTexcoord);

    objectColor.rgb = pow(objectColor.rgb, vec3(kInverseGamma));

    //ambient
    vec3 ambient = kAmbientStrength * kLightColor;
    //diffuse
    vec3 N = normalize(vNormal);
    vec3 L = normalize(uLightPos - vFragPos);
    float diff_factor = max(dot(N, L), 0.0);
    vec3 diffuse = diff_factor * kLightColor;

    //specular
    vec3 V = normalize(uEyePos - vFragPos);
    vec3 R = reflect(-L, N);

    float spe_factor = pow(max(dot(V, R), 0.0), uShiness);
    vec3 specular = kSpecularStrength * spe_factor * kLightColor;
//    //    gl_FragColor = vec4(vec3(specular), 1.0);
    vec4 phongColor= vec4(( ambient + diffuse + specular) * objectColor.rgb ,1.0);

    vec3 correctColor = pow(phongColor.rgb, vec3(kGamma));
    correctColor*= uColorCorrection.rgb * (uColorCorrection.a/kMiddleGrayGamma);
    gl_FragColor = vec4(correctColor.rgb, phongColor.a);
}