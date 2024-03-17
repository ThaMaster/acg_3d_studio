#version 410 core

layout (location = 0) out vec4 color;

in vec2 texCoords;

uniform sampler2D scene;
uniform sampler2D sceneBlur;
uniform sampler2D depthTexture;
uniform sampler2D bloomBlur;

uniform bool useDOF;
uniform bool bloom;

void main()
{
    if (useDOF) {
        float maxFocalDist = 0.97;
        float minFocalDist = 0.8;

        // Sample depth value from the depth texture
        float depthValue = texture(depthTexture, texCoords).r;

        // Calculate the depth difference from the focal plane
        float depthDifference = abs(depthValue - minFocalDist);

        // Calculate the maximum possible depth difference in the scene
        float maxDepthDifference = maxFocalDist - minFocalDist;

        // Normalize the depth difference
        float normalizedDepthDifference = depthDifference / maxDepthDifference;

        // Calculate the blur amount based on normalized depth difference
        float blurAmount = smoothstep(0.0, 1.0, normalizedDepthDifference);

        // Mix between the regular scene and the blurry scene based on blur amount
        vec4 sceneColor = texture(scene, texCoords);
        vec4 blurColor = texture(sceneBlur, texCoords);
        vec4 dofResult = mix(sceneColor, blurColor, blurAmount);


        // Apply bloom effect if enabled
        if (bloom) {
            vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
            dofResult.rgb += bloomColor;
        }

        color = dofResult;
    } else {
        // If depth of field is not enabled, just use the regular scene
        vec3 sceneColor = texture(scene, texCoords).rgb;

        // Apply bloom effect if enabled
        if (bloom) {
            vec3 bloomColor = texture(bloomBlur, texCoords).rgb;
            sceneColor += bloomColor;
        }

        color = vec4(sceneColor, 1.0);
    }
}
