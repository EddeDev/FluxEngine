#stage vertex
#version 450 core

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec4 a_Color;

uniform mat4 u_ProjectionMatrix;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec4 v_Color;

void main()
{
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;

    gl_Position = u_ProjectionMatrix * vec4(a_Position.xy, 0.0, 1.0);
}

#stage fragment
#version 450 core

layout (location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec4 v_Color;

uniform sampler2D u_Texture;

void main()
{
    o_Color = v_Color * texture(u_Texture, v_TexCoord);
}