#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    uint useTexture;
} ubo;

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
   // outColor = texture(texSampler, fragTexCoord);
    if (ubo.useTexture == 1) {
        outColor = texture(texSampler, fragTexCoord); // Glass container
    } else {
        outColor = vec4(1.0, 0.0, 0.0, 1.0); // Particle
    }
}