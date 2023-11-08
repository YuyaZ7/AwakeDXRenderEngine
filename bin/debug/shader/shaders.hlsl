#include "Lighting.hlsl"
#include "sampler.hlsl"
struct VSInput {
  float3 PositionL : POSITION;
  float3 NormalL : NORMAL;
  float2 uv : TEXCOORD0;
};
struct PSInput {
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float2 uv : TEXCOORD0;
};

cbuffer MVPBuffer : register(b0) { float4x4 m_MVP; }
cbuffer Lightbuffer : register(b1) {
  uint directNum;
  uint pointNum;
  uint spotNum;
  uint padding0;
  float3 gEyePos;
  uint padding1;
  DirLight gDirLights[MAX_DIR];
  PointLight gPointLights[MAX_POINT];
}
Texture2D baseTexture : register(t0);

PSInput VSMain(VSInput input) {
  float3 viewDir = normalize(input.PositionL - gEyePos);
  float3 outputColor = float3(0, 0, 0);

  // Direct
  // for (uint i = 0; i < directNum; i++) {
  //   float3 lightDirection = -normalize(gDirLights[i].Direction);
  //   float3 halfDir = normalize(lightDirection + viewDir);
  //   float specAngle = max(dot(halfDir, input.NormalL), 0);
  //   outputColor += specAngle * gDirLights[i].Strength;
  // }
  // for (uint k = 0; k < pointNum; k++) {
  //   float3 lightDirection =
  //       normalize(gPointLights[k].Position - input.PositionL);
  //   float3 halfDir = normalize(lightDirection + viewDir);
  //   float specAngle = max(dot(halfDir, input.NormalL), 0);
  //   outputColor += specAngle * gPointLights[k].Strength;
  // }
  PSInput result;
  result.position = mul(m_MVP, float4(input.PositionL, 1));
  result.uv = input.uv;
  // result.color = float4(outputColor, 1);
  return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
  input.color = baseTexture.Sample(gsamLinearClamp, input.uv, 0);
  return input.color;
}
