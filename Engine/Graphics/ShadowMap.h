#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Texture)
#pragma endregion

CLASS_PTR(ShadowMap)
class ShadowMap
{
public:
	static ShadowMapUPtr Create(int32 width, int32 height);
	~ShadowMap();

	const uint32 Get() const { return m_framebuffer; }
	void Bind() const;
	const TexturePtr GetShadowMap() const { return m_shadowMap; }

private:
	ShadowMap();
	bool Init(int32 width, int32 height);

	uint32 m_framebuffer{ 0 };
	TexturePtr m_shadowMap;
};