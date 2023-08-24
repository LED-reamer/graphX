#shader vertex
#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aRadius;
layout (location = 3) in vec2 aCenter;

out vec4 color;
out float radius;
out vec2 center;

uniform mat4 u_mvp;


void main()
{
	color = aColor;
	radius = aRadius;
	center = aCenter;
	gl_Position = u_mvp * vec4(aPos, 0.0, 1.0);
}



#shader fragment
#version 330 core

in vec4 color;
in float radius;
in vec2 center;


out vec4 FragColor;


void main()
{
	vec4 ret = color;
	float distanceToCenter = distance(gl_FragCoord.xy, center);
	if (!(distanceToCenter < radius))
	{
		discard;
	}
	ret.a = smoothstep(radius, radius - 2, distanceToCenter);
	FragColor = ret;
}