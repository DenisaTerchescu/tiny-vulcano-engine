#version 450

layout(location = 0) in vec3 fragColor;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) out vec4 outColor;

void main() {

    outColor = vec4(1.0, 0.0, 0.0, 1.0); 
    
}