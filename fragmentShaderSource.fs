#version 440 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

//Classes for specific types of light and materials
struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct PointLight {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quad;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//uniform variable for the implemented texture 
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;
uniform sampler2D ourTexture3;
uniform sampler2D ourTexture4;

uniform vec3 viewPos;

uniform Material material; //Material object
uniform DirLight dirLight; //Light object

//Creates the capability to create multiple point lights in the scene
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

//function call with a vec3 return
vec3 dirLightObj(DirLight light, vec3 normal, vec3 viewDir);
vec3 pointLightObj(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    //normalize all vectors 
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 lightResult = dirLightObj(dirLight, norm, viewDir);

         lightResult += pointLightObj(pointLights[0], norm, FragPos, viewDir);

    FragColor = mix(texture(ourTexture, TexCoord), texture(ourTexture2,TexCoord), 0.3) * vec4(lightResult * ourColor, 1.0); 
}

vec3 dirLightObj(DirLight light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = normalize(-light.direction);

    float diff = max(dot(normal, lightDir), 0.0); //uses dot product to calculate lighting diffuse - set max values to prevent neg lighting
    vec3 relfectDir = reflect(-lightDir, normal); //calculates the reflection direction for an incident vector

    float spec = pow(max(dot(viewDir, relfectDir), 0.0), material.shininess);

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoord));

    return (ambient + diffuse + specular);
}

vec3 pointLightObj(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 relfectDir = reflect(-lightDir, normal);

    float spec = pow(max(dot(viewDir, relfectDir), 0.0), material.shininess);

    //attenuation reduces the intensity of the light through source distance
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quad * (distance * distance));

    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoord));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}