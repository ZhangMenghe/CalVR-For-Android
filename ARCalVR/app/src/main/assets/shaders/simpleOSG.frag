uniform vec4 lightColor;
uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform float shininess;

uniform vec4 uColorCorrection;
uniform vec4 uColor;
uniform float alphaValue;

varying vec3 normal, eyeVec, lightDir;

void main() {

    const float kGamma = 0.4545454;
    const float kMiddleGrayGamma = 0.466;
    vec4 finalColor = vec4(0.0,0.0,0.0,1.0);

    // adding diffuse color
    float lambert = max(dot(normal, lightDir), 0.0);
    vec3 irradiance = uColor.rgb + lightColor.rgb * lambert;
    finalColor.rgb += irradiance * lightDiffuse.rgb;

    // adding specular color
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-lightDir, normal);
    float specular = pow(max(dot(R, E), 0.0), shininess);
    finalColor += lightSpecular * specular;


    // Apply SRGB gamma before writing the fragment color.
//    finalColor.rgb = pow(finalColor.rgb, vec3(kGamma)); // makes everything more muted

    // Apply average pixel intensity and color shift
//    finalColor.rgb *= uColorCorrection.rgb * (uColorCorrection.a/kMiddleGrayGamma);
    gl_FragColor = vec4(finalColor.rgb, alphaValue);

}
