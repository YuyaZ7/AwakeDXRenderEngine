#include "Lighting.hlsl"
#include "sampler.hlsl"

struct VSInput {
  float3 PositionL : POSITION;
  float3 NormalL : NORMAL;
  float2 uv : TEXCOORD0;
};

struct PSInput {
  float4 position : SV_POSITION;
  float3 normal : NORMAL;
  float2 uv : TEXCOORD1;
  float3 worldPos : TEXCOORD2;
};

cbuffer MVPBuffer : register(b0) { float4x4 m_MVP; }

cbuffer Lightbuffer : register(b1) {
  uint directNum;
  uint pointNum;
  uint spotNum;
  uint padding0; // Test the padding stuff later
  float3 gEyePos;
  uint padding1;
  DirLight gDirLights[MAX_DIR];
  PointLight gPointLights[MAX_POINT];
}

cbuffer ItemBuffer : register(b2) {
  bool hasDiffTexture;
  bool hasBumbTexture;
  bool hasSpecTexture;
  bool hasMaskTexture;
  float diffColor;
  float specIntensity;
}

Texture2D baseTexture : register(t0);
Texture2D bumpTexture : register(t1);
Texture2D specTexture : register(t2);
Texture2D maskTexture : register(t3);
Texture2D shadowMap : register(t4);
// Array of unknown number of shadow maps, later implement

PSInput VSMain(VSInput input) {
  PSInput result;
  result.position = mul(float4(input.PositionL, 1.f), m_MVP);
  result.uv = input.uv;
  result.normal = input.NormalL;
  result.worldPos = input.PositionL;
  return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
  float3 result = 0.f;
  float3 baseColor = baseTexture.Sample(gsamLinearWrap, input.uv).xyz;
  float3 envLight = float3(0.2f, 0.2f, 0.2f);
  float3 viewDir = normalize(gEyePos - input.worldPos);

  for (uint i = 0; i < directNum; i++) {
    float3 lightDirection = -normalize(gDirLights[i].Direction);
    float3 halfDir = normalize(lightDirection + viewDir);
    float specAngle = 0.f; // max(dot(halfDir, input.normal), 0);
    float3 diffuse = max(dot(input.normal, lightDirection), 0);
    float3 lightColor = gDirLights[i].Strength * (specAngle + diffuse);
    float shadow = 0.f;

    // Transform fragment position to light space
    float4 posLightSpace =
        mul(float4(input.worldPos, 1.f), gDirLights[i].lightViewProjMatrix);
    posLightSpace.xyz /= posLightSpace.w;

    float2 shadowTexCoord = float2(0.f, 0.f);
    shadowTexCoord.x = 0.5f * posLightSpace.x + 0.5f;
    shadowTexCoord.y = -0.5f * posLightSpace.y + 0.5f;

    if (posLightSpace.x < -1.0f || posLightSpace.x > 1.0f ||
        posLightSpace.y < -1.0f || posLightSpace.y > 1.0f ||
        posLightSpace.z < 0.0f || posLightSpace.z > 1.0f) {
      continue;
    }

    float shadowDepth = shadowMap.Sample(gsamPointWrap, shadowTexCoord).r;

    // Check if current fragment is in shadow
    float bias = 0.02;
    if (posLightSpace.z - bias > shadowDepth) {
      shadow = 1.0f;
    }

    result += lightColor * baseColor * (1 - shadow);
  }

  for (uint k = 0; k < pointNum; k++) {
    float3 lightDirection =
        normalize(gPointLights[k].Position - input.worldPos);
    float3 halfDir = normalize(lightDirection + viewDir);
    float specAngle = max(dot(halfDir, input.normal), 0);
    float3 diffuse = max(dot(input.normal, lightDirection), 0);
    result += (specAngle + diffuse) * gPointLights[k].Strength * baseColor;
  }

  result += baseColor * envLight;
  return float4(result, 1.f);
}
