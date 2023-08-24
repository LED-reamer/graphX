#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in float aTextureSlot;
layout(location = 4) in vec4 aSrcRect;

out vec4 color;
out vec2 texCoord;
out float textureSlot;
out vec4 srcRect;

uniform mat4 u_mvp;


void main()
{
	color = aColor;
	texCoord = aTexCoord;
	textureSlot = aTextureSlot;
	srcRect = aSrcRect;
	gl_Position = u_mvp * vec4(aPos, 1.0);
}



#shader fragment
#version 330 core

out vec4 FragColor;

in vec4 color;
in vec2 texCoord;
in float textureSlot;
in vec4 srcRect;

uniform sampler2D u_textures[32];

void main()
{
	int textureIndex = int(textureSlot);

	vec2 adjustedTexCoord = vec2(srcRect.x + (srcRect.z * texCoord.x), srcRect.y + (srcRect.w * texCoord.y));

	FragColor = texture(u_textures[textureIndex], adjustedTexCoord) * color;
}