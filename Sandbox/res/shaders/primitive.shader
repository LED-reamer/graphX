#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;

out vec4 color;

uniform mat4 u_mvp;


void main()
{
	color = aColor;
	gl_Position = u_mvp * vec4(aPos, 0.0, 1.0);
}



#shader fragment
#version 330 core

in vec4 color;


out vec4 FragColor;


void main()
{
	FragColor = color;
}