#stage vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;

layout(location = 0) out vec3 v_Normal;
layout(location = 1) out vec3 v_WorldPosition;

/*
layout(push_constant) uniform Transform
{
	mat4 Transform;
} u_Constants;
*/
uniform mat4 u_Transform;
uniform mat4 u_ViewProjMatrix;

void main()
{
    v_Normal = mat3(u_Transform) * a_Normal;
    v_WorldPosition = vec3(u_Transform * vec4(a_Position, 1.0));

    gl_Position = u_ViewProjMatrix * u_Transform * vec4(a_Position, 1.0);
}

#stage fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_Normal;
layout(location = 1) in vec3 v_WorldPosition;

void main()
{
    const vec3 lightPos = vec3(2000, 1000, -2000);
    const vec3 lightColor = vec3(1.0);

    const vec3 objectColor = vec3(0.6, 0.4, 0.2);

    vec3 lightDir = normalize(lightPos - v_WorldPosition);

    vec3 normal = normalize(v_Normal);

    vec3 ambient = 0.1 * lightColor;
    vec3 diffuse = max(dot(normal, lightDir), 0.0) * lightColor;

    vec3 finalColor = (ambient + diffuse) * objectColor;

    o_Color = vec4(finalColor, 1.0);
}