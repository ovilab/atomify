#version 330 core

#pragma shadernodes header

in vec3 vs_vertex1Position;
in vec3 vs_vertex2Position;

in vec3 modelPosition;
in vec3 modelViewPosition;
in vec3 base;
in vec3 end;
in vec3 color;
in float radiusA;
in float radiusB;
in float da;
in mat3 cylinderBasis;
in mat3 cylinderWorldBasis;

in vec3 v1;
in vec3 v2;

in vec3 perpendicular;
in vec3 biperpendicular;

out vec4 fragColor;

uniform vec3 eyePosition;
uniform mat3 modelNormalMatrix;
uniform mat3 modelViewNormal;

float square(vec3 a) {
    return dot(a, a);
}

void main(void) {
    vec3 rayOrigin = vec3(0.0, 0.0, 0.0); // in modelview space
    vec3 rayTarget = modelViewPosition; // in modelview space
    vec3 rayDirection = normalize(rayOrigin - rayTarget);

    mat3 basis = cylinderBasis; // cylinder space basis
    vec3 axis = cylinderBasis[2];
    vec3 worldAxis = cylinderWorldBasis[2];

    // Ray target in cylinder space
    vec3 cylTarget = rayTarget * basis;

    // Direction from base to hitpoint in model view space
    vec3 diff = rayTarget - base;

    // Ray origin in cylinder basis
    vec3 E = diff * basis;

    // radius and length of cone
    float r1 = radiusA;
    float r2 = radiusB;
    float rd = r2 - r1;
    float l = length(base - end);

    // Ray direction in cylinder basis
    vec3 D = rayDirection * basis;

    // Cone equation is
    //     x^2 + y^2 = z^2
    // However, we modify this to start with r1 and end up with r2
    //      x^2 + y^2 = (r1 + rd/l * z)^2
    // where l is the length of the cylinder along the z axis
    // and rd = r2 - r1.
    // Ray equation is
    //     P(t) = E + t*D
    // We substitute ray into cylinder equation to get
    //     (Ex + Dx * t)^2 + (Ey + Dy * t)^2 = (r1 + rd / l * (Ez + Dz * t))^2
    // The result is a quadratic equation with form
    //      a*t^2 + b*t + c = 0
    // where
    //      a = Dx^2 + Dy^2 - (...)
    //      b = 2*Dx*Ex + 2*Dy*Ey - (...)
    //      c = Ex^2 + Ey^2 - (...)
    float a = D.x*D.x + D.y*D.y - (rd*rd/(l*l)*D.z*D.z);
    float b = 2.0*E.x*D.x + 2.0*E.y*D.y - (2.0*rd*rd/(l*l)*E.z*D.z + 2*r1*rd/l*D.z);
    float c = E.x*E.x + E.y*E.y - (r1*r1 + rd*rd/(l*l)*E.z*E.z + 2*r1*rd/l*E.z);
    // the solutions are
    //      t = (-b +/- sqrt(b*b - 4*a*c)) / 2*a
    // the discriminant of the above equation is
    //      d = b*b - 4*a*c
    float d = b*b - 4.0*a*c;
    // if this is below zero, there is no solution
    // and we are outside cylinder

    if (d < 0.0) {
        //        fragcolor = vec4(0.2, 1.0, 0.2, 1.0);
        //        return;
        discard;
    }

    // solution t
    float distSide = (-b + sqrt(d))/(2.0*a);

    // ray-plane intersection
    // d = ((p0 - E) . n) / (D . n)
    float distBase = dot(base - rayTarget, axis) / dot(rayDirection, axis);
    float distEnd = dot(end - rayTarget, axis) / dot(rayDirection, axis);

    // produce the possible points of the solutions
    vec3 sidePoint = rayTarget + distSide * rayDirection;
    vec3 basePoint = rayTarget + distBase * rayDirection;
    vec3 endPoint = rayTarget + distEnd * rayDirection;

    float baseDistance = square(basePoint - base);
    float endDistance = square(endPoint - end);

    float screenSideDistance = square(sidePoint);
    float screenBaseDistance = square(basePoint);
    float screenEndDistance = square(endPoint);

    bool isSideSolution = true;

    vec3 normal;
    float dist = distSide;


    // first, test if the end cap planes are closer to the
    // screen than the solution to the cone equation
    // this is unlikely, but is necessary to ensure that we
    // don't get solutions beyond the cone epicenter ><
    if((screenBaseDistance <= screenSideDistance && screenBaseDistance <= screenEndDistance) && baseDistance < r1*r1) {
        dist = distBase;
        isSideSolution = false;
        normal = -worldAxis;
    }
    if(screenEndDistance <= screenSideDistance && screenEndDistance <= screenBaseDistance && endDistance < r2*r2) {
        dist = distEnd;
        isSideSolution = false;
        normal = worldAxis;
    }

    if(isSideSolution) {
        // we didn't find a cap plane that was closer,
        // so we check if the point on the side is outside the
        // length of the cylinder, i.e. outside the two caps.
        // To do this, we check the angle between the point of
        // intersection - cylinder end vector and a cap plane normal
        // (which is the cylinder axis)
        // if cos angle < 0, the point is outside of cylinder
        if(dot(sidePoint - base, axis) < 0.0) {
            dist = distBase;
            isSideSolution = false;
            normal = -worldAxis;
            if(baseDistance > r1*r1) {
                discard;
            }
        }
        if(dot(sidePoint - end, -axis) < 0.0) {
            dist = distEnd;
            isSideSolution = false;
            normal = worldAxis;
            if(endDistance > r2*r2) {
                discard;
            }
        }
    }

    // use the found solution to produce the final version
    // of the point in cylinder space
    vec3 cylPoint = E + D * dist;
    vec3 cylPointWorld = vs_vertex1Position + cylinderWorldBasis * cylPoint;

    // use the perpendicular vector to define the texture coordinate
    vec3 xyPoint = vec3(cylPoint.x, cylPoint.y, 0.0);
    vec3 xyPointWorld = cylinderWorldBasis * xyPoint;
    float x = dot(xyPointWorld, perpendicular);
    float y = dot(xyPointWorld, biperpendicular);
    float radius = r1 + rd * cylPoint.z / l; // interpolates from r1 to t2 by z
    vec2 texCoord = vec2(0.5 + x / (2.0*radius), 0.5 + y / (2.0*radius));

    if(isSideSolution) {
        // we didn't find a solution that returned one of the caps
        // and must calculate the normal.
        // the normal is the gradient of the function defining the cone shape
        // This is found in cone space as
        //      grad = vec3(2.0*x, 2.0*y, -(rd/l*r1 + 2*rd^2/l^2*z))
        vec3 cylNormal = vec3(2.0*cylPoint.x,
                              2.0*cylPoint.y,
                              (2.0*(r1-r2)*(r1*(l - cylPoint.z) + r2*cylPoint.z))/(l*l));
        // we transform this to real space by using our basis matrix
        normal = cylinderWorldBasis * cylNormal;
        normal = normalize(normal);

        float pi = 3.1415926535897932384626433832795;
        float z = cylPoint.z / l;
        texCoord = vec2(z, 0.5 + atan(y, x) / (2.0 * pi));
    }

    // calculate texture coordinate
    vec3 position = cylPointWorld;

#pragma shadernodes body

}
