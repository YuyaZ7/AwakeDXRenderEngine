#include <DXMath/MathHelper.h>
#include <Resource/Texture.h>

class PointLight {
public:
	static uint GetRawSize();
	PointLight(XMFLOAT3 intensity, XMFLOAT3 position)
		: intensity(intensity), padding0(0), position(position), padding1(0){};
	XMFLOAT3 intensity;
	uint padding0;//memory alignment
	XMFLOAT3 position;
	uint padding1;
	struct RawData {
		XMFLOAT3 Strength;
		float padding0;
		XMFLOAT3 Position;
		float padding1;
	};
	RawData GetRawData();
};
