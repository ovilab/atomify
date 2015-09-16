in vec2 texCoord;
in vec3 vertexPosition;
in float da;
out vec4 fragcolor;

void main(void) {
    vec3 color = vec3(0.0, 1.0, 1.0);
    highp float x = texCoord.s;
    highp float y = texCoord.t;
    float yy = y*y;
    if(yy > 0.9) {
        discard;
    }

    float dada = da*da;
    if(x>(1.0-da) ) {
        float d = 1.0 - x;
        // if(d*d/da2 + y*y < 1.0) { discard; }
        if(d*d + yy*dada < dada) { discard; }
    } else if(x<0.0) {
        float d = -x;
        // if(d*d/da2 + y*y > 1.0) { discard; }
        if(d*d + yy*dada > dada) { discard; }
    }

    highp float z = sqrt(1.0 - yy);
    highp vec3 light = vec3(1.0, 1.0, 1.0)*z;
    fragcolor = vec4(color*light, 1.0);
}
