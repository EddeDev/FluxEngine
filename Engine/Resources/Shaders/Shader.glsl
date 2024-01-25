#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Color;

layout(location = 0) out vec3 v_Color;

/*
layout(push_constant) uniform Transform
{
	mat4 Transform;
} u_Constants;
*/
uniform mat4 u_Transform;

void main()
{
    v_Color = a_Color;
    gl_Position = u_Transform * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_Color;

void main()
{
    o_Color = vec4(v_Color, 1.0);
}