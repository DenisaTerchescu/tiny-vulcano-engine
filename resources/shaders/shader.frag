#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;

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
   // if (ubo.useTexture == 1) {
    //    outColor = texture(texSampler, fragTexCoord); // Glass container
  //  } else {
    //    outColor = vec4(1.0, 0.0, 0.0, 1.0); // Particle
   // }

   vec3 ambientLight = vec3(0.2, 0.0, 0.1);
      vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5)); // Light direction
    float diff = max(dot(normalize(fragNormal), lightDir), 0.0); // Diffuse lighting factor
     vec3 lightColor = vec3(1.0, 0.75, 0.8);
    vec3 lighting = (diff * lightColor); // Light intensity and color

    vec4 texColor = texture(texSampler, fragTexCoord); // Texture color
    vec3 finalColor = lighting * texColor.rgb; // Combine lighting with texture

    outColor = vec4(finalColor, texColor.a); // Output color
}