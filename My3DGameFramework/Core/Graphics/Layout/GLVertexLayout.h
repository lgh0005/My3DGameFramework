#pragma once

namespace MGF3D
{
	MGF_CLASS_PTR(GLVertexLayout)
	class GLVertexLayout
	{
	public:
		~GLVertexLayout();
		static GLVertexLayoutUPtr Create();

	public:
		uint32 GetHandle() const;
		void Bind() const;
		void Unbind() const;

	public:
		void SetAttribFormat(uint32 attribIndex, int32 count, uint32 type, bool normalized, uint32 relativeOffset, uint32 bindingIndex = 0) const;
		void SetAttribIFormat(uint32 attribIndex, int32 count, uint32 type, uint32 relativeOffset, uint32 bindingIndex = 0) const;
		void SetBindingDivisor(uint32 bindingIndex, uint32 divisor) const;

	public:
		void EnableAttrib(uint32 attribIndex) const;
		void DisableAttrib(uint32 attribIndex) const;

	private:
		GLVertexLayout();
		void Init();
		uint32 m_handle{ 0 };
	};
}