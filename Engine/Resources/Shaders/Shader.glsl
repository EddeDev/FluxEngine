#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

struct VertexOutput
{
    vec3 WorldPosition;

    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;

    vec2 TexCoord;
};

layout(location = 0) out VertexOutput Output;

uniform mat4 u_Transform;
uniform mat4 u_ViewProjectionMatrix;

void main()
{
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0));
    
    Output.Normal = mat3(u_Transform) * a_Normal;
    Output.Tangent = a_Tangent;
    Output.Binormal = a_Binormal;

    Output.TexCoord = a_TexCoord;

    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
    vec3 WorldPosition;

    vec3 Normal;
    vec3 Tangent;
    vec3 Binormal;

    vec2 TexCoord;
};

layout(location = 0) in VertexOutput Input;

void main()
{
    const vec3 lightPos = vec3(2000, 2500, -1000);
    const vec3 lightColor = vec3(1.0);

    const vec3 objectColor = vec3(1.0);

    vec3 lightDir = normalize(lightPos - Input.WorldPosition);

    vec3 normal = normalize(Input.Normal);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

    vec3 finalColor = (ambient + diffuse) * objectColor;

    o_Color = vec4(finalColor, 1.0);
}