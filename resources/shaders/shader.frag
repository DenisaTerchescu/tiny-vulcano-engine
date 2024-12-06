#version 450

layout(location = 0) in vec3 fragPosition;
layout(location = 1) in vec3 fragColor;
layout(location = 2) in vec2 fragTexCoord;
layout(location = 3) in vec3 fragNormal;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 viewPos;
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
vec3 lightPos = normalize(vec3(10, 1.0, 1.0)); 
vec3 lightDir = normalize(lightPos - fragPosition); 
vec3 lightColor = vec3(1.0, 0.75, 0.8); 
vec3 objectColor = vec3(0.0, 1.0, 0.0);

// ambient light
vec3 ambientColor = vec3(0.1, 0.1, 0.1);

// diffuse light
float diff = max(dot(fragNormal, lightDir), 0.0);
vec3 diffuseLighting = diff * lightColor;

// specular light
float specularStrength = 0.5;
vec3 viewDir = normalize(ubo.viewPos - fragPosition);
vec3 reflectDir = reflect(-lightDir, fragNormal); 
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
vec3 specular = specularStrength * spec * lightColor;  

vec3 finalColor = (diffuseLighting + ambientColor + specular) * objectColor;
outColor = vec4(finalColor, 1.0); 

}
