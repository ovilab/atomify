in vec2 texCoord;
in vec3 vertexPosition;
in float da;
in float mode;
out vec4 fragcolor;
void main(void) {
    vec3 color = vec3(0.0, 1.0, 1.0);
    highp float x = texCoord.s;
    highp float y = texCoord.t;
    float yy = y*y;
    if(yy > 0.9) {
        discard;
    }
    float da2 = da*da;
    if(mode > 0.5) {
        // v1 closer than v2
        if(x>1.0) {
            float d = x - 1.0;
            if(d*d/da2 + y*y > 1.0) { discard; }
        } else if(x<0.0) {
            float d = x + da;
            if(d*d/da2 + y*y < 1.0) { discard; }
        }
    } else {
        // v2 closer than v1
        if(x>1.0) {
            float d = 1.0 + da - x;
            if(d*d/da2 + y*y < 1.0) { discard; }
        } else if(x<0.0) {
            float d = -x;
            if(d*d/da2 + y*y > 1.0) { discard; }
        }
    }

    highp float z = sqrt(1.0 - y*y);
    highp vec3 light = vec3(1.0, 1.0, 1.0)*z;
    fragcolor = vec4(color*light, 1.0);
}
