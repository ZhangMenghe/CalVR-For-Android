uniform vec4 uColor;
uniform float alphaValue;

varying vec3 frag_normal, eyeVec, lightDir;
uniform vec4 uColorCorrection;

uniform vec3 lightColor = vec3(1);
uniform vec3 diffuseColor = vec3(0.5);
uniform vec4 specularColor = vec4(0.8, 0.8, 0.8, 1.0);
uniform float shininess = 30.0f;

void main() {

    const float kGamma = 0.4545454;
    const float kInverseGamma = 2.2;
    const float kMiddleGrayGamma = 0.466;
    vec4 finalColor = vec4(1.0,.0,.0,1.0);
    finalColor.rgb = pow(finalColor.rgb, vec3(kInverseGamma));

    float lambert = max(dot(frag_normal, lightDir), 0.0);
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-lightDir, frag_normal);
    float specular = pow(max(dot(R, E), 0.0), shininess);
    finalColor += specularColor * specular;

    vec3 color = finalColor.rgb * uColor.rgb;
        // Apply SRGB gamma before writing the fragment color.
        color.rgb = pow(color, vec3(kGamma));
        // Apply average pixel intensity and color shift
        color *= uColorCorrection.rgb * (uColorCorrection.a/kMiddleGrayGamma);
    gl_FragColor = vec4(color.rgb, alphaValue);


    // FROM IK PROJECT
//    // Compute irradiance (sum of ambient & direct lighting)
//    vec3 irradiance = uColor + lightColor * max(dot(frag_normal, lightDir), 0.0);
//
//    // Diffuse reflectance
//    vec3 reflectance = irradiance * diffuseColor;
//
//    // Gamma correction
//    gl_FragColor = vec4(sqrt(reflectance), alphaValue);
//
//    // adding in specular
//    vec3 R = reflect(-lightDir, fragNormal);
//    vec3 e = normalize(eyeVec);
//    gl_FragColor += pow(max(dot(R, e), 0), shininess);
}
