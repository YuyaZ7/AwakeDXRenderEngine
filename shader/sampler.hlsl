SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearPointClamp : register(s1);
SamplerState gsamLinearClamp : register(s2);
SamplerState gsamAnisotropicClamp : register(s3);

SamplerState gsamPointWrap : register(s4);
SamplerState gsamLinearPointWrap : register(s5);
SamplerState gsamLinearWrap : register(s6);
SamplerState gsamAnisotropicWrap : register(s7);

SamplerState gsamPointMirror : register(s8);
SamplerState gsamLinearPointMirror : register(s9);
SamplerState gsamLinearMirror : register(s10);
SamplerState gsamAnisotropicMirror : register(s11);

SamplerState gsamPointBorder : register(s12);
SamplerState gsamLinearPointBorder : register(s13);
SamplerState gsamLinearBorder : register(s14);
SamplerState gsamAnisotropicBorder : register(s15);