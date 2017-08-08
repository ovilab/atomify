highp vec3 makePerpendicular(highp vec3 v) {
    if(v.x == 0.0 && v.y == 0.0) {
        if(v.z == 0.0) {
            return vec3(0.0, 0.0, 0.0);
        }
        return vec3(0.0, 1.0, 0.0);
    }
    return vec3(-v.y, v.x, 0.0);
}

highp float linearizeDepth(highp sampler2D depthTexture, highp vec2 texCoord) {
    highp float z = texture(depthTexture, texCoord).x * 2.0 - 1.0;
    highp float m22 = -projectionMatrix[2][2];
    highp float m32 = -projectionMatrix[3][2];

    highp float nearPlane = m32 / (m22 + 1.0);
    highp float farPlane = m32 / (m22 - 1.0);

    highp float n = nearPlane;
    highp float f = farPlane;

    return 2.0 * n * f / (f + n - z * (f - n));
}

highp vec2 texCoordFromPosition(highp vec3 position, highp mat4 viewMatrix, highp mat4 projectionMatrix) {
    highp vec4 projectedPosition = projectionMatrix * viewMatrix * vec4(position.xyz, 1.0);
    projectedPosition.xy /= projectedPosition.w;
    projectedPosition.xy = projectedPosition.xy * 0.5 + 0.5;
    return projectedPosition.xy;
}

// TODO something is wrong with this implementation, should be tested with simple objects like spheres and cubes
highp float ambientOcclusion(highp sampler2D depthTexture, highp sampler2D noiseTexture, highp sampler2D randomVectorTexture,
                             highp vec3 position, highp vec3 inNormal, highp vec2 noiseTexCoord,
                             highp int samples, highp float radius, highp float noiseScale, highp int modeType,
                             highp float cutoff, highp float contrast,
                             highp mat4 viewMatrix, highp  mat4 projectionMatrix) {
    highp vec2 positionTexCoord = texCoordFromPosition(position, viewMatrix, projectionMatrix);
    highp float fragDepth = linearizeDepth(depthTexture, positionTexCoord);

    highp vec3 normal = normalize(inNormal);
    highp vec3 randomVector = normalize(-1.0 + 2.0 * texture(noiseTexture, noiseTexCoord * noiseScale).rgb);

    highp vec3 tangent = normalize(randomVector - normal * dot(randomVector, normal));
    highp vec3 bitangent = normalize(cross(normal, tangent));
    highp mat3 basis = mat3(tangent, bitangent, normal);

    highp float occlusion = 0.0;
    for(int i = 0; i < samples; i++) {
        highp vec3 sampleRay = normalize(basis*normalize(-1.0 + 2.0 * texelFetch(randomVectorTexture, ivec2(i, modeType), 0).rgb));
        if(abs(dot(sampleRay, normal)) < 0.15) {
            continue;
        }
        sampleRay *= radius;
        highp vec3 samplePosition = position.xyz + sampleRay;

        highp vec2 texCoord = texCoordFromPosition(samplePosition, viewMatrix, projectionMatrix);
        highp float sampleDepth = linearizeDepth(depthTexture, texCoord);
        if(abs(sampleDepth - fragDepth) <= cutoff * radius && sampleDepth < fragDepth) {
            occlusion += pow(fragDepth - sampleDepth, contrast);
        }
    }

    return max(0.0, (1.0 - occlusion / float(samples)));
}

