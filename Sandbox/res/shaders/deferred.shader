#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

//gbuffer data
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

//lights
struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    int state;
};
struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    int state;
};
struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    int state;
};

#define MAX_POINT_LIGHTS 32
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
//uniform SpotLight spotLight;


uniform vec3 viewPos;



vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 Albedo, float Specular);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 Albedo, float Specular);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir, vec3 Albedo, float Specular);


void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    vec3 viewDir = normalize(viewPos - FragPos);


    vec3 result = Albedo;
    if(dirLight.state != 0)
        result = CalcDirLight(dirLight, Normal, viewDir, Albedo, Specular);

    for (int i = 0; i < MAX_POINT_LIGHTS; i++)
    {
        if (pointLights[i].state == 0) continue;//test if light is essentially null
        result += CalcPointLight(pointLights[i], Normal, FragPos, viewDir, Albedo, Specular);
    }
    //if (spotLight.state != 0)//test if light is essentially null
    //    result += CalcSpotLight(spotLight, Normal, FragPos, viewDir, Albedo, Specular);

    FragColor = vec4(result, 1);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 Albedo, float Specular)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), /*material.shininess*/16)/2;//Temp const
    // combine results
    vec3 ambient = light.ambient * Albedo;
    vec3 diffuse = light.diffuse * diff * Albedo;
    vec3 specular = light.specular * spec * Specular;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir, vec3 Albedo, float Specular)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8);//temp const
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * Albedo;
    vec3 diffuse = light.diffuse * diff * Albedo;
    vec3 specular = light.specular * spec * Specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 FragPos, vec3 viewDir, vec3 Albedo, float Specular)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 1;//pow(max(dot(viewDir, reflectDir), 0.0), /*material.shininess*/16);//Temp const
    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * Albedo;
    vec3 diffuse = light.diffuse * diff * Albedo;
    vec3 specular = light.specular * spec * Specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}