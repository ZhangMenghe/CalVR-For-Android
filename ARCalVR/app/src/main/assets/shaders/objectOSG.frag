uniform vec4 lightDiffuse;
uniform vec4 lightSpecular;
uniform float shininess;
uniform vec4 uColorCorrection;
uniform sampler2D uSampler, uSamplerC;
uniform bool uTextureChoice;

varying vec3 normal, eyeVec, lightDir;
varying vec2 vTexCoord;

void main(){
    const float kGamma = 0.4545454;
    const float kInverseGamma = 2.2;
    const float kMiddleGrayGamma = 0.466;
    vec4 finalColor = vec4(1.0,.0,.0,1.0);
    if(uTextureChoice)
        finalColor = texture2D(uSampler, vTexCoord);
    else
        finalColor = texture2D(uSamplerC, vTexCoord);
    finalColor.rgb = pow(finalColor.rgb, vec3(kInverseGamma));

    vec3 N = normalize(normal);
    vec3 L = normalize(lightDir);
    float lambert = max(dot(N,L), 0.0);
    vec3 E = normalize(eyeVec);
    vec3 R = reflect(-L, N);
    float specular = pow(max(dot(R, E), 0.0), shininess);
    finalColor += lightSpecular * specular;

    vec3 color = finalColor.rgb;
        // Apply SRGB gamma before writing the fragment color.
        color.rgb = pow(color, vec3(kGamma));
        // Apply average pixel intensity and color shift
        color *= uColorCorrection.rgb * (uColorCorrection.a/kMiddleGrayGamma);
    gl_FragColor = vec4(color.rgb, finalColor.a);


}