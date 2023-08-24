#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    TexCoord = aTexCoord;

    mat3 normalMatrix = transpose(inverse(mat3(model)));
    Normal = normalMatrix * aNormal;

    gl_Position = projection * view * worldPos;
}

#shader fragment
#version 330 core
layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct Material
{
    sampler2D colorTexture;
    sampler2D normalTexture;
    sampler2D roughnessTexture;
};
uniform Material material;

void main()
{
    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal) * normalize(texture(material.normalTexture, TexCoord).rgb * 2.0 - 1.0);
    // and the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(material.colorTexture, TexCoord).rgb;
    // store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(material.roughnessTexture, TexCoord).a;
}