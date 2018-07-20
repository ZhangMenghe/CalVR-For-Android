precision mediump float;
uniform vec4 uLightingParams;
uniform vec4 uMaterialParams;
uniform vec4 uColorCorrection;

varying vec2 v_texCoord;
varying vec3 v_viewNormal;
varying vec3 v_viewPosition;

uniform sampler2D uTexture;

void main() {
	gl_FragColor = texture2D(uTexture, v_texCoord);
//    // We support approximate sRGB gamma.
//    const float kGamma = 0.4545454;
//    const float kInverseGamma = 2.2;
//    const float kMiddleGrayGamma = 0.466;
//
//    // Unpack lighting and material parameters for better naming.
//    vec3 viewLightDirection = uLightingParams.xyz;
//    vec3 colorShift = uColorCorrection.rgb;
//    float averagePixelIntensity = uColorCorrection.a;
//
//    float materialAmbient = uMaterialParams.x;
//    float materialDiffuse = uMaterialParams.y;
//    float materialSpecular = uMaterialParams.z;
//    float materialSpecularPower = uMaterialParams.w;
//
//    // Normalize varying parameters, because they are linearly interpolated in
//    // the vertex shader.
//    vec3 viewFragmentDirection = normalize(v_ViewPosition);
//    vec3 viewNormal = normalize(v_ViewNormal);
//
//    // Apply inverse SRGB gamma to the texture before making lighting
//    // calculations.
//    // Flip the y-texture coordinate to address the texture from top-left.
//    vec4 objectColor = texture2D(uTexture,
//            vec2(v_TexCoord.x, 1.0 - v_TexCoord.y));
//    objectColor.rgb = pow(objectColor.rgb, vec3(kInverseGamma));
//
//    // Ambient light is unaffected by the light intensity.
//    float ambient = materialAmbient;
//
//    // Approximate a hemisphere light (not a harsh directional light).
//    float diffuse = materialDiffuse *
//            0.5 * (dot(viewNormal, viewLightDirection) + 1.0);
//
//    // Compute specular light.
//    vec3 reflectedLightDirection = reflect(viewLightDirection, viewNormal);
//    float specularStrength = max(0.0, dot(viewFragmentDirection,
//            reflectedLightDirection));
//    float specular = materialSpecular *
//            pow(specularStrength, materialSpecularPower);
//
//    vec3 color = objectColor.rgb * (ambient + diffuse) + specular;
//    // Apply SRGB gamma before writing the fragment color.
//    color.rgb = pow(color, vec3(kGamma));
//    // Apply average pixel intensity and color shift
//    color *= colorShift * (averagePixelIntensity/kMiddleGrayGamma);
//    gl_FragColor.rgb = color;
//    gl_FragColor.a = objectColor.a;
}
