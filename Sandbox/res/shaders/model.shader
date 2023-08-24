#shader vertex
#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
//layout(location = 2) in vec4 aTangent;

out vec3 Normal;
out vec2 TexCoord;

uniform mat4 u_mvp;

void main()
{
    Normal = aNormal;
    TexCoord = aTexCoord;

    gl_Position = u_mvp * vec4(aPosition, 1.0);
}



#shader fragment
#version 330 core

in vec3 Normal;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D u_colorTex;
uniform sampler2D u_normalTex;


void main()
{
    float diff = max(dot(texture(u_normalTex, TexCoord).rgb, vec3(.78, -.9, -.22)), 0.0);
    vec4 diffuse = diff * vec4(.3, .2, .2, 0);
    
    FragColor = texture(u_colorTex, TexCoord) - diffuse;
}
