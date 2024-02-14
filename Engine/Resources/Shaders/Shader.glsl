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
    mat3 TBN;
    vec2 TexCoord;
    mat3 ViewMatrix;
    vec3 ViewPosition;
};

layout(location = 0) out VertexOutput Output;

uniform mat4 u_Transform;

uniform mat4 u_ViewProjectionMatrix;
uniform mat4 u_ViewMatrix;

void main()
{
    Output.WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0));
    Output.Normal = mat3(u_Transform) * a_Normal;
    Output.TBN = mat3(u_Transform) * mat3(a_Tangent, a_Binormal, a_Normal);

#define FLIP_TEXTURE 0
#if FLIP_TEXTURE
    Output.TexCoord = vec2(a_TexCoord.x, 1.0 - a_TexCoord.y);
#else
    Output.TexCoord = a_TexCoord;
#endif

    Output.ViewMatrix = mat3(u_ViewMatrix);
    Output.ViewPosition = vec3(u_ViewMatrix * vec4(Output.WorldPosition, 1.0));

    gl_Position = u_ViewProjectionMatrix * u_Transform * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

struct VertexOutput
{
    vec3 WorldPosition;
    vec3 Normal;
    mat3 TBN;
    vec2 TexCoord;
    mat3 ViewMatrix;
    vec3 ViewPosition;
};

layout(location = 0) in VertexOutput Input;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetalnessMap;

uniform vec4 u_AlbedoColor;
uniform float u_Roughness;
uniform float u_Metalness;
uniform float u_Emission;

uniform vec3 u_LightColor;
uniform vec3 u_LightPosition;
uniform float u_AmbientMultiplier;

uniform uint u_HasNormalMap;

vec3 CalculateNormal()
{
    vec3 normal = normalize(Input.Normal);
    if (u_HasNormalMap == 1)
    {
        normal = texture(u_NormalMap, Input.TexCoord).rgb;
        normal = normalize(normal * 2.0 - 1.0);

        // Convert to world space
        normal = normalize(Input.TBN * normal);
    }
    return normal;
}

void main()
{
    vec3 normal = CalculateNormal();
    
    vec3 objectColor = texture(u_AlbedoMap, Input.TexCoord).rgb * u_AlbedoColor.rgb;

    vec3 lightDirection = normalize(u_LightPosition - Input.WorldPosition);

    vec3 ambient = u_AmbientMultiplier * u_LightColor;
    vec3 diffuse = max(dot(normal, lightDirection), 0.0) * u_LightColor;

    vec3 finalColor = (ambient + diffuse) * objectColor;

    if (false)
    {
        float frequency = 0.02;
        float gray = 0.9;
        
        vec2 v1 = step(0.5, fract(frequency * gl_FragCoord.xy));
        vec2 v2 = step(0.5, vec2(1.0) - fract(frequency * gl_FragCoord.xy));
        finalColor *= gray + v1.x * v1.y + v2.x * v2.y;
    }

    o_Color = vec4(finalColor, 1.0);
}