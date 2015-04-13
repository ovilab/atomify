// BEGIN billboards.fsh
uniform sampler2D texture;
varying highp vec2 coords;
varying highp vec3 color;
varying highp float attenuationFactor;
varying highp vec3 vertexPosition;
varying highp float billboardId;

void main() {
    highp vec2 textureCoordinate = vec2(0.5*coords.s, coords.t);
    highp vec4 picture =  texture2D(texture, textureCoordinate);
    highp vec3 light = vec3(1.0, 1.0, 1.0);

    //convert to range -1.0 to 1.0
    highp vec2 normalMapTextureCoordinate = vec2((1.0 + coords.s)*0.5, coords.t);
    highp vec3 sphereNormal = texture2D(texture, normalMapTextureCoordinate).rgb * 2.0 - 1.0;

#ifdef DEFAULTLIGHT
    highp vec3 normal = sphereNormal.x*cp_rightVector - sphereNormal.y*cp_upVector - sphereNormal.z*cp_viewVector;

#ifdef SIMPLEXBUMP
    normal = simplexbump(normal, normal+vec3(billboardId));
#endif

    light = vec3(ambient(cp_ambientColor) + attenuationFactor*(diffuse(normal, vertexPosition, cp_diffuseColor) + specular(normal, vertexPosition, cp_specularColor)));
#endif

    if(picture.a < 0.9) {
        discard;
    }

    gl_FragColor = picture*vec4(color*light, 1.0);
}
// END billboards.fsh
