in vec2 texCoord;
in vec3 vertexPosition;
in float da;
in vec3 yDir;
out vec4 fragcolor;

void main(void) {
    vec3 color = vec3(0.0, 1.0, 1.0);
    highp float x = texCoord.s;
    highp float y = texCoord.t;
    float yy = y*y;
    if(yy > 1.0) {
        discard;
    }

    float dada = da*da;
    if(x<0.0) {
        if(x*x + yy*dada > dada) { discard; }
    }

    highp vec3 light = vec3(1.0, 1.0, 1.0);
    highp float z = sqrt(1.0 - yy);

    highp vec3 normal = y*yDir - z*cp_viewVector;
#ifdef DEFAULTLIGHT
    light = defaultLight(normal, vertexPosition, color);
#endif
    // light = vec3(1.0, 1.0, 1.0)*z*z;
    fragcolor = vec4(color*light, 1.0);
}
