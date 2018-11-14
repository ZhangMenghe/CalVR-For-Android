uniform sampler2D uSampler, uSamplerC;
uniform bool uTextureChoice;

varying vec4 vColor;
varying vec2 vTexCoord;

void main(){
    vec4 finalColor;
    if(uTextureChoice)
        finalColor = texture2D(uSampler, vTexCoord);
    else
        finalColor = texture2D(uSamplerC, vTexCoord);
    gl_FragColor = vColor * finalColor;// *
}