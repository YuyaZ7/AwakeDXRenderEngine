struct VSInput {
  float3 PositionL : POSITION;
};
struct PSInput {
  float4 pos : SV_POSITION;
};
cbuffer lightInfo : register(b0) { float4x4 lightViewProj; }

PSInput VSMain(VSInput input) {
  PSInput output;
  output.pos = mul(float4(input.PositionL, 1.f), lightViewProj);
  return output;
}
void PSMain(PSInput input) {}