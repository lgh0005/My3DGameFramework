#include "CorePch.h"
#include "GLVertexLayout.h"
#include "Graphics/Buffers/GLVertexBuffer.h"
#include "Graphics/Buffers/GLIndexBuffer.h"

namespace MGF3D
{
	GLVertexLayout::GLVertexLayout() = default;
	GLVertexLayout::~GLVertexLayout()
	{
		if (m_handle != 0)
		{
			glDeleteVertexArrays(1, &m_handle);
			m_handle = 0;
		}
	}

	GLVertexLayoutUPtr GLVertexLayout::Create()
	{
		auto layout = GLVertexLayoutUPtr(new GLVertexLayout());
		layout->Init();
		return layout;
	}

	void GLVertexLayout::Init()
	{
		glCreateVertexArrays(1, &m_handle);
	}

	uint32 GLVertexLayout::GetHandle() const
	{
		return m_handle;
	}

	void GLVertexLayout::Bind() const
	{
		glBindVertexArray(m_handle);
	}

	void GLVertexLayout::Unbind() const
	{
		glBindVertexArray(0);
	}

	void GLVertexLayout::SetAttribFormat(uint32 attribIndex, int32 count, uint32 type, bool normalized, uint32 relativeOffset, uint32 bindingIndex) const
	{
		glEnableVertexArrayAttrib(m_handle, attribIndex);
		glVertexArrayAttribFormat(m_handle, attribIndex, count, type, CommonUtils::Select(normalized, GL_TRUE, GL_FALSE), relativeOffset);
		glVertexArrayAttribBinding(m_handle, attribIndex, bindingIndex);
	}

	void GLVertexLayout::SetAttribIFormat(uint32 attribIndex, int32 count, uint32 type, uint32 relativeOffset, uint32 bindingIndex) const
	{
		glEnableVertexArrayAttrib(m_handle, attribIndex);
		glVertexArrayAttribIFormat(m_handle, attribIndex, count, type, relativeOffset);
		glVertexArrayAttribBinding(m_handle, attribIndex, bindingIndex);
	}

	void GLVertexLayout::SetBindingDivisor(uint32 bindingIndex, uint32 divisor) const
	{
		glVertexArrayBindingDivisor(m_handle, bindingIndex, divisor);
	}

	void GLVertexLayout::EnableAttrib(uint32 attribIndex) const
	{
		glEnableVertexArrayAttrib(m_handle, attribIndex);
	}

	void GLVertexLayout::DisableAttrib(uint32 attribIndex) const
	{
		glDisableVertexArrayAttrib(m_handle, attribIndex);
	}

	void GLVertexLayout::BindVertexBuffer(uint32 bindingIndex, const GLVertexBufferPtr& buffer, uint32 offset, uint32 stride) const
	{
		glVertexArrayVertexBuffer(m_handle, bindingIndex, buffer->GetHandle(), offset, stride);
	}

	void GLVertexLayout::BindIndexBuffer(const GLIndexBufferPtr& buffer) const
	{
		glVertexArrayElementBuffer(m_handle, buffer->GetHandle());
	}


	void GLVertexLayout::SetAttribBinding(uint32 attribIndex, uint32 bindingIndex) const
	{
		glVertexArrayAttribBinding(m_handle, attribIndex, bindingIndex);
	}
}