#pragma shadernodes header

uniform highp vec4 lightPosition;
uniform highp vec3 lightIntensity;

uniform highp vec3 ka;                            // Ambient reflectivity
uniform highp vec3 ks;                            // Specular reflectivity
uniform highp float shininess;            // Specular shininess factor

varying highp vec3 position;
varying highp vec3 color;
varying highp vec2 planePosition;

// out vec4 fragColor;

uniform highp mat4 modelView;
uniform highp vec3 eyePosition;

uniform highp vec3 viewVector;
uniform highp vec3 rightVector;
uniform highp vec3 upVector;

highp vec3 adsModel( const highp vec3 pos, const highp vec3 n )
{
    // Calculate the vector from the light to the fragment
//    vec3 s = normalize( vec3( lightPosition ) - pos );
    highp vec3 lightpos = eyePosition;
    highp vec3 s = normalize(vec3(lightpos) - pos);

    // Calculate the vector from the fragment to the eye position
    // (origin since this is in "eye" or "camera" space)
    highp vec3 v = normalize( eyePosition-pos );

    // Calculate the diffuse component
    highp float diffuse = max( 0.0, dot( n, s ) );

    // Combine the ambient, diffuse and specular contributions
    return lightIntensity * ( ka + diffuse );
}

void main(void) {
    highp float x = planePosition.s;
    highp float y = planePosition.t;
    highp float r2 = x*x + y*y;
    if(r2 > 1.0) {
        discard;
    } else {
        highp float z = sqrt(1.0 - r2); // Equation for sphere, x^2 + y^2 + z^2 = R^2
        highp vec3 normal = x*rightVector + y*upVector - z*viewVector;
        highp float pi = 3.1415926535897932384626433832795;
        highp vec2 texCoord = vec2(0.5 + atan(-normal.z, normal.x) / (2.0 * pi), 0.5 - asin(normal.y) / pi);
        highp vec4 fragColor = vec4(1.0, 1.0, 1.0, 1.0);
//        fragColor = vec4( adsModel( position, normalize( normal ) ), 1.0 );
#pragma shadernodes body

        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        gl_FragColor = fragColor;
    }
}
