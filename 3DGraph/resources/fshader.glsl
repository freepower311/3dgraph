#version 130

uniform sampler2D texture;
uniform sampler2D shadowMapTex;
uniform samplerCube environmentMap;
uniform mat4 inverseViewNormalMatrix;
uniform vec4 viewSpaceLightPosition;
uniform vec3 scene_ambient_light = vec3(0.1);
uniform vec3 scene_light = vec3(0.6, 0.6, 0.6);
uniform vec3 material_diffuse_color = vec3(1.0);
uniform vec3 material_specular_color = vec3(0.9);
uniform vec3 material_emissive_color = vec3(0.0);
uniform float material_shininess = 25.0;
uniform vec3 viewSpaceLightDir;
uniform float spotOpeningAngle;
in vec2 texCoord;
in vec4 viewSpaceNormal;
in vec4 viewSpacePosition;
in vec4 shadowMapCoord;
out vec4 fragColor;

vec3 calculateAmbient(vec3 ambientLight, vec3 materialAmbient)
{
    return ambientLight * materialAmbient;
}

vec3 calculateDiffuse(vec3 diffuseLight, vec3 materialDiffuse, vec3 normal, vec3 directionToLight)
{
    return diffuseLight*materialDiffuse*max(0,dot(normal,directionToLight));
}

vec3 calculateSpecular(vec3 specularLight, vec3 materialSpecular, float materialShininess, vec3 normal, vec3 directionToLight, vec3 directionFromEye)
{
    vec3 h = normalize(directionToLight - directionFromEye);

    return specularLight*materialSpecular*pow(max(0,dot(h,normal)), materialShininess);
}

vec3 calculateFresnel(vec3 materialSpecular, vec3 normal, vec3 directionFromEye)
{
    return materialSpecular+(vec3(1.0)-materialSpecular)*pow(clamp(1.0 + dot(directionFromEye,normal),0.0,1.0),5.0);
}

vec3 sampleDiffuseTexture()
{
    return texture2D(texture, texCoord).xyz;
}

void main()
{
    vec3 normal = normalize(viewSpaceNormal.xyz);
    vec3 directionToLight = normalize(viewSpaceLightPosition.xyz - viewSpacePosition.xyz);
    vec3 directionFromEye = normalize(viewSpacePosition.xyz);
    vec3 specular = material_specular_color;
    float normalizationFactor = (material_shininess + 2.0) / 8.0;

    vec4 reflectionVector = inverseViewNormalMatrix * vec4(reflect(directionFromEye, normal), 0.0);
    vec3 envMapSample = 0.5*textureCube(environmentMap, reflectionVector.xyz).rgb;


    float depth = texture2D(shadowMapTex, shadowMapCoord.xy / shadowMapCoord.w ).x;
    float visibility = (depth >= (shadowMapCoord.z/shadowMapCoord.w))? 1.0 : 0.0;

    float angle = dot(directionToLight,-viewSpaceLightDir);
    float spotAttenuation = (angle > spotOpeningAngle) ? 1.0 : 0.0;

    vec3 ambient = material_diffuse_color * sampleDiffuseTexture();
    vec3 diffuse = sampleDiffuseTexture() * material_diffuse_color;
    vec3 emissive = sampleDiffuseTexture() * material_emissive_color;
    vec3 shading = calculateAmbient(scene_ambient_light,ambient)
            + emissive
            + visibility*calculateDiffuse(scene_light, diffuse, normal, directionToLight)
            + visibility*calculateSpecular(scene_light, specular, material_shininess,normal, directionToLight, directionFromEye)
            + visibility*envMapSample*calculateFresnel(specular, normal, directionFromEye)*sampleDiffuseTexture();


    fragColor = vec4(shading, 1.0);
    //fragColor = vec4(spotAttenuation*shading, 1.0);
    //fragColor = texture2D(shadowMapTex, texCoord);
    //fragColor = vec4(visibility);
    //fragColor.w = 1.0;
}
