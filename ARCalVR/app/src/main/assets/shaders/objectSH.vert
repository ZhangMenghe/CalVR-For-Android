uniform float uLightScale; // Scale the diffuse color
uniform vec3 uSHBasis[9];

varying vec4 vColor;
varying vec2 vTexCoord;

const float C1 = 0.429043;
const float C2 = 0.511664;
const float C3 = 0.743125;
const float C4 = 0.886227;
const float C5 = 0.247708;

void main(){
    vec3 tnorm = gl_NormalMatrix * gl_Normal;;
    vec3 DiffuseColor =  C1 * uSHBasis[8] * (tnorm.x * tnorm.x - tnorm.y * tnorm.y) +
                   C3 * uSHBasis[6] * tnorm.z * tnorm.z +
                   C4 * uSHBasis[0] -
                   C5 * uSHBasis[6] +
                   2.0 * C1 * uSHBasis[4] * tnorm.x * tnorm.y +
                   2.0 * C1 * uSHBasis[7] * tnorm.x * tnorm.z +
                   2.0 * C1 * uSHBasis[5] * tnorm.y * tnorm.z +
                   2.0 * C2 * uSHBasis[3] * tnorm.x +
                   2.0 * C2 * uSHBasis[1] * tnorm.y +
                   2.0 * C2 * uSHBasis[2] * tnorm.z;
    DiffuseColor *= uLightScale;
    vColor.rgb = DiffuseColor;
    vColor.a = 1.0;
    gl_Position =  gl_ModelViewProjectionMatrix* gl_Vertex;
    vTexCoord = gl_MultiTexCoord0.xy;
}