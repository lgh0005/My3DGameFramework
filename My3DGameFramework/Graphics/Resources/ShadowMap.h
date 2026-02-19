#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(ShadowMap)
class ShadowMap
{
public:
	static ShadowMapUPtr Create(int32 resolution);
	~ShadowMap();

	void Bind() const;
	const uint32 Get() const { return m_framebuffer; }
	const TexturePtr GetShadowMap() const { return m_shadowMap; }
	int32 GetResolution() const { return m_resolution; }
	bool Resize(int32 resolution);

private:
	ShadowMap();
	bool Init(int32 resolution);
	bool CreateAttachment(int32 resolution);

	uint32 m_framebuffer{ 0 };
	int32 m_resolution{ 0 };
	TexturePtr m_shadowMap;
};