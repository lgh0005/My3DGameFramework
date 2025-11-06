#pragma once

#pragma region FORWARD_DECLARATION
CLASS_PTR(Image)
#pragma endregion

CLASS_PTR(Texture)
class Texture
{
public:
	static TextureUPtr CreateFromImage(const Image* image);
	~Texture();

	const uint32 Get() const { return m_texture; }
	void Bind() const;
	void SetFilter(uint32 minFilter, uint32 magFilter) const;
	void SetWrap(uint32 sWrap, uint32 tWrap) const;

private:
	Texture() = default;
	void CreateTexture();
	void SetTextureFromImage(const Image* image);

	uint32 m_texture	{ 0 };
};