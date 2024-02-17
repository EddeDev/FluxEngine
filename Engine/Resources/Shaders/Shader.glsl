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

const float PI = 3.1415926535897932;
const float Epsilon = 0.00001;

const vec3 Fdielectric = vec3(0.04);

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
uniform vec3 u_LightDirection;
uniform vec3 u_CameraPosition;
uniform float u_AmbientMultiplier;

uniform uint u_HasNormalMap;

struct
{
    vec3 AlbedoColor;
    float Roughness;
    float Metalness;
    vec3 Normal;

    vec3 ViewDirection;
    vec3 F0;
    float NdotV;
} m_Params;

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

float DistributionGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

float GeometrySchlickGGX(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

float GeometrySmith(float cosLi, float cosLo, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;
    return GeometrySchlickGGX(cosLi, k) * GeometrySchlickGGX(cosLo, k);
}

vec3 FresnelSchlick(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 DirectionalLight(vec3 direction, vec3 color)
{
    vec3 Li = -direction;
    vec3 Lh = normalize(Li + m_Params.ViewDirection);

    float cosLi = max(dot(m_Params.Normal, Li), 0.0);
    float cosLh = max(dot(m_Params.Normal, Lh), 0.0);

    vec3 F = FresnelSchlick(m_Params.F0, max(dot(Lh, m_Params.ViewDirection), 0.0), m_Params.Roughness);
    float D = DistributionGGX(cosLh, m_Params.Roughness);
    float G = GeometrySmith(cosLi, m_Params.NdotV, m_Params.Roughness);

    vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
    vec3 diffuseBRDF = kd * m_Params.AlbedoColor;

    vec3 specularBRDF = (F * D * G) / max(Epsilon, 4.0 * cosLi * m_Params.NdotV);
    specularBRDF = clamp(specularBRDF, vec3(0.0), vec3(10.0));

    return (diffuseBRDF + specularBRDF) * color * cosLi;
}

const vec3 SkyColor = vec3(0.0);

vec3 AmbientLighting()
{
    vec3 F = FresnelSchlick(m_Params.F0, m_Params.NdotV, m_Params.Roughness);
    vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
    vec3 diffuseIBL = m_Params.AlbedoColor * SkyColor;
    return kd * diffuseIBL;
}

void main()
{
    // Parameters
    m_Params.AlbedoColor = texture(u_AlbedoMap, Input.TexCoord).rgb * u_AlbedoColor.rgb;
    m_Params.Metalness = texture(u_MetalnessMap, Input.TexCoord).r * u_Metalness;
    m_Params.Roughness = texture(u_RoughnessMap, Input.TexCoord).r * u_Roughness;
    m_Params.Normal = CalculateNormal();

    m_Params.ViewDirection = normalize(u_CameraPosition - Input.WorldPosition);
    m_Params.NdotV = max(dot(m_Params.Normal, m_Params.ViewDirection), 0.0);

    m_Params.F0 = mix(Fdielectric, m_Params.AlbedoColor, m_Params.Metalness);

    vec3 color = vec3(0.0);
    color += DirectionalLight(u_LightDirection, u_LightColor);
    color += AmbientLighting() * u_AmbientMultiplier;
    
	{
		float d = length(Input.ViewPosition);
		float density = 0.015;
		float gradient = 1.5;
		float v = exp(-pow(d * density, gradient));
		v = clamp(v, 0.0, 1.0);

		color = mix(SkyColor * u_AmbientMultiplier, color, v);
	}

    color = pow(color, vec3(1.0 / 2.2));

    o_Color = vec4(color, 1.0);
}