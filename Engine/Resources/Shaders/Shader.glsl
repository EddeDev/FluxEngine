#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;
layout(location = 3) in float a_TextureIndex;

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;
layout(location = 2) out flat float v_TextureIndex;

layout(std140, binding = 0) uniform Camera
{
	mat4 u_ViewProjectionMatrix;
	mat4 u_InverseViewProjectionMatrix;
	mat4 u_ProjectionMatrix;
	mat4 u_ViewMatrix;
};

layout(push_constant) uniform Uniforms
{
	mat4 Transform;
} u_Uniforms;

void main()
{
    Output.Color = a_Color;
    Output.TexCoord = a_TexCoord;
    v_TextureIndex = a_TextureIndex;

    gl_Position = u_ViewProjectionMatrix * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
    vec4 Color;
    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;
layout(location = 2) in flat float v_TextureIndex;

void main()
{
    vec4 color = Input.Color;

	if (color.a == 0.0)
		discard;

	o_Color = color;
}