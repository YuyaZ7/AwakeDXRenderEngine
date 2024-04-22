#include <DXMath/MathHelper.h>

#include <Resource/ShadowMap.h>

class DirectionalLight {

public:
	DirectionalLight(const XMFLOAT3& intensity, const XMFLOAT3& position, const XMFLOAT3& right, const XMFLOAT3& forward, const XMFLOAT3& up, uint shadowResolution, Device* device, bool isCastShadow);
	void SetDirection(XMFLOAT3 newDir);
	void SetPosition(XMFLOAT3 newPos);
	void SetIntensity(XMFLOAT3 newInten) { intensity = newInten; };
	void UpdateLightProjMatrix();
	void UpdateLightViewMatrix();
	void LookAt(const Math::Vector3& pos, const Math::Vector3& target, const Math::Vector3& worldUp);
	void CreateView4ShadowMap(DescriptorHeap* dsvHeap, DescriptorHeap* srvHeap, uint miplevels);

	Math::Matrix4 GetViewMatrix() const;
	Math::Matrix4 GetProjectionMatrix() const;
	Math::Matrix4 GetViewProjectionMatrix() const;
	ShadowMap* GetShadowMap() const;

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetDirection() const;

	static uint GetRawSize() {
		return sizeof(RawData);
	}
	struct RawData {
		XMFLOAT3 Strength;
		bool isCastShadow;
		XMFLOAT3 Direction;
		float padding0;
		Math::Matrix4 lightViewProjMatrix;
	};
	RawData GetRawData();

private:
	XMFLOAT3 position;
	XMFLOAT3 forward;
	XMFLOAT3 intensity;
	XMFLOAT3 up;
	XMFLOAT3 right;
	Math::Matrix4 lightViewMatrix;
	Math::Matrix4 lightProjMatrix;
	ShadowMap* shadowMap;

	float shadowResolution;
	float orthoWidth = 4000.f;
	float orthoHeight = 4000.f;
	float zn = 1.f;
	float zf = 4000.f;
	bool isCastShadow;
};