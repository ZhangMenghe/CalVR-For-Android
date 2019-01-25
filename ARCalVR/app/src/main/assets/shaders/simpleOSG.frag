uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform float shininess;
uniform vec4 uColorCorrection;

uniform vec4 uColor;
uniform float alphaValue;

varying vec3 normal, eyeVec, lightDir;

void main() {

    // Menghe's objectOSG
    const float kGamma = 0.4545454;
    const float kInverseGamma = 2.2;
    const float kMiddleGrayGamma = 0.466;
    vec4 finalColor = vec4(1.0,1.0,1.0,1.0);
    finalColor.rgb = pow(finalColor.rgb, vec3(kInverseGamma));

    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    float lambert = max(dot(N,L), 0.0);
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-L, N);
    float specular = pow(max(dot(R, E), 0.0), shininess);
    finalColor += lightSpecular * specular;

    vec3 color = finalColor.rgb * uColor.rgb;
    // Apply SRGB gamma before writing the fragment color.
    color.rgb = pow(color, vec3(kGamma));
    // Apply average pixel intensity and color shift
    color *= uColorCorrection.rgb * (uColorCorrection.a/kMiddleGrayGamma);
    gl_FragColor = vec4(color.rgb, alphaValue);


    // FROM IK PROJECT
//    vec3 lightColor = vec3(1, 1, 1);
//    vec3 diffuseColor = vec3(0.5, 0.5, 0.5);
//    vec4 specularColor = vec4(0.8, 0.8, 0.8, 1.0);
//    float shininess = 30.0f;
//
//    // normalize the normal and light directions
//    normal = normalize(normal);
//    lightDir = normalize(lightDir);
//
//    // Compute irradiance (sum of ambient & direct lighting)
//    vec3 irradiance = uColor.rgb * max(dot(normal, lightDir), 0.0);
//
//    // Diffuse reflectance
//    vec3 reflectance = irradiance * uColor.rgb;
//
//    // Gamma correction
//    vec4 finalcolor;
//    finalcolor = vec4(sqrt(reflectance), alphaValue);
//
//    // adding in specular
//    vec3 R = reflect(-lightDir, normal);
//    vec3 e = normalize(eyeVec);
//    gl_FragColor += pow(max(dot(R, e), 0), shininess);
}
