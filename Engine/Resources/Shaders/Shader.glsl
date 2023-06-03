#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

struct VertexOutput
{
    vec3 Position;
    vec4 Color;
};

layout(location = 0) out VertexOutput Output;

layout(push_constant) uniform Constants
{
	mat4 ProjectionMatrix;
} u_Constants;

void main()
{
    Output.Position = a_Position;
    Output.Color = a_Color;

    gl_Position = u_Constants.ProjectionMatrix * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
    vec3 Position;
    vec4 Color;
};

layout(location = 0) in VertexOutput Input;

// layout(set = 1, binding = 5) uniform sampler2D u_Texture;

void main()
{
    // o_Color = Input.Color * texture(u_Texture, Input.Color.xy);
   o_Color = Input.Color;
}